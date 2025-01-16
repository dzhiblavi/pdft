#include "pdft/app/loader/impl/app_persister.h"
#include "pdft/app/state/ui.h"

#include "platform/log.h"
#include "platform/module/register.h"

#include <rfl/json.hpp>

namespace pdft::app {

using namespace platform;

namespace {

struct State {
    UIModel ui;
    std::vector<std::string> paths;
};

}  // namespace

AppPersister::AppPersister(
    platform::Arc<UI> ui,
    platform::Arc<DocumentStorage> documents,
    platform::Arc<const media::Loader> doc_loader)
    : ui_(std::move(ui))
    , documents_(std::move(documents))
    , doc_loader_(std::move(doc_loader)) {}

platform::Status AppPersister::save(std::ostream& os) const {
    State state;
    state.ui = ui_->getModel();

    for (const auto& doc : state.ui.documents) {
        auto id = doc.document_id;
        auto document = documents_->getDocument(id) | result::unwrap;
        state.paths.push_back(document->getPath());
    }

    rfl::json::write(state, os);
    return result::unit;
}

platform::Status AppPersister::restore(std::istream& is) const {
    auto maybe_state = rfl::json::read<State>(is);
    if (!maybe_state) {
        return result::makeError<std::string>("failed to read state");
    }

    auto&& state = maybe_state.value();
    auto& ui = ui_->getModel();
    ui = state.ui;

    std::vector<size_t> to_be_removed;
    for (size_t i = 0; i < state.paths.size(); ++i) {
        auto&& path = state.paths[i];
        auto maybe_document = doc_loader_->load(path);

        if (!maybe_document) {
            to_be_removed.push_back(i);
            log::warn(
                this,
                "failed to load file by path while restoring state path={}: {}",
                path,
                maybe_document.error<std::string>());
            continue;
        }

        auto document = *maybe_document;
        auto page_count = document->getPageCount();
        auto id = documents_->addDocument(document);
        ui_->onDocumentChanged(ui.documents[i].document_id, id, page_count);
    }

    std::ranges::reverse(to_be_removed);
    for (size_t i : to_be_removed) {
        // XXX fix this hack
        ui.index = 0;
        ui.documents.erase(ui.documents.begin() + static_cast<long>(i));
    }

    return result::unit;
}

}  // namespace pdft::app

PLATFORM_REGISTER_MODULE(pdft::app::AppPersister);
