#include "pdft/app/loader/impl/loader.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include <magic_enum.hpp>

namespace pdft::app::loader::impl {

namespace {

bool isOpened(
    const std::string& abs_path,
    const state::UIState& ui,
    const state::DocumentStorage& documents) {
    // XXX this does not actually require linear search

    for (const auto& state : ui.states) {
        auto maybe_doc = documents.getDocument(state.document_id);
        PDFT_VERIFY(maybe_doc);
        auto doc = *maybe_doc;

        if (abs_path == doc->getPath()) {
            return true;
        }
    }

    return false;
}

}  // namespace

void Loader::save(
    const state::UIState& ui, const state::DocumentStorage& documents, std::ostream& os) {
    persister_->save(ui, documents, os);
    log::info("the state has been successfully saved");
}

Result<void> Loader::load(
    const std::vector<std::string>& paths,
    state::UIState& ui,
    state::DocumentStorage& documents,
    StateMode mode,
    std::istream& is) {
    auto mode_name = magic_enum::enum_name(mode);

    if (mode != StateMode::Standalone && mode != StateMode::Clean) {
        pdft::log::info("loading the state because mode is {}", mode_name);

        if (auto result = persister_->restore(ui, documents, is); !result) {
            pdft::log::warn("failed to restore the state: {}; ignoring", result.error());
        } else {
            pdft::log::info("the state has been successfully restored");
        }
    }

    for (auto path : paths) {
        path = std::filesystem::absolute(path);

        if (isOpened(path, ui, documents) && mode == StateMode::AddOnly) {
            log::info("ignoring the new path {} because state mode is AddOnly", path);
            continue;
        }

        auto maybe_document = doc_loader_->load(path);
        if (!maybe_document) {
            log::warn("failed to open document by path {}: {}", path, maybe_document.error());
            continue;
        }

        auto document = *maybe_document;
        auto page_count = document->getPageCount();
        auto doc_id = documents.addDocument(document);
        ui.addDocument(doc_id, page_count);
        log::info("loaded path {}", path);
    }

    return {};
}

}  // namespace pdft::app::loader::impl
