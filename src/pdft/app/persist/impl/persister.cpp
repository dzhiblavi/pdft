#include "pdft/app/persist/impl/persister.h"
#include "pdft/core/log/log.h"

#include <rfl/json.hpp>

#include <vector>

namespace pdft::app::persist::impl {

namespace {

struct State {
    UIState ui;
    std::vector<std::string> paths;
};

}  // namespace

void Persister::save(const UIState& ui, const DocumentStorage& documents, std::ostream& os) const {
    State state;
    state.ui = ui;

    for (const auto& doc : ui.states) {
        auto id = doc.document_id;
        auto document = documents.getDocument(id) | result::unwrap;
        state.paths.push_back(document->getPath());
    }

    rfl::json::write(state, os);
}

Result<void> Persister::restore(UIState& ui, DocumentStorage& documents, std::istream& is) const {
    auto maybe_state = rfl::json::read<State>(is);

    if (!maybe_state) {
        return result::err(makeErrorCode(PersisterError::FailedToReadState));
    }

    auto&& state = maybe_state.value();
    ui = state.ui;

    std::vector<size_t> to_be_removed;
    for (size_t i = 0; i < state.paths.size(); ++i) {
        auto&& path = state.paths[i];
        auto maybe_document = doc_loader_->load(path);

        if (!maybe_document) {
            to_be_removed.push_back(i);
            log::warn(
                "failed to load file by path while restoring state path={}: {}",
                path,
                maybe_document.error());
            continue;
        }

        auto document = *maybe_document;
        auto page_count = document->getPageCount();
        auto id = documents.addDocument(document);

        ui.states[i].setDocument(id, page_count);
    }

    std::ranges::reverse(to_be_removed);
    for (size_t i : to_be_removed) {
        // XXX fix this hack
        ui.idx = 0;
        ui.states.erase(ui.states.begin() + i);
    }

    return {};
}

}  // namespace pdft::app::persist::impl
