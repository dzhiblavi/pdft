#include "pdft/app/state_loader/impl/app_state_loader.h"

#include "pdft/core/log.h"
#include "pdft/core/module/register.h"
#include "pdft/core/result.h"

#include "pdft/utils/cli_args/cli_args.h"
#include "pdft/utils/get_home_dir.h"

#include <magic_enum.hpp>

namespace pdft::app {

namespace {

bool isOpened(const std::string& abs_path, const UIState& ui, const DocumentStorage& documents) {
    // XXX this does not actually require linear search

    for (const auto& state : ui.states) {
        auto doc = documents.getDocument(state.document_id) | result::unwrap;

        if (abs_path == doc->getPath()) {
            return true;
        }
    }

    return false;
}

struct CliArgs {
    std::optional<StateMode> state_mode;
};

}  // namespace

AppStateLoader::AppStateLoader(
    Config config,
    std::span<const char*> argv,
    inject<const Persister, "persister"> persister,
    inject<const media::Loader, "media_loader"> doc_loader)
    : mode_(config.mode)
    , persister_(std::move(persister))
    , doc_loader_(std::move(doc_loader)) {
    auto cli = utils::parseFromCliArgs<CliArgs>(argv) | result::unwrap;
    mode_ = cli.state_mode.value_or(mode_);
    log::info(this, "AppStateLoader state mode is {}", magic_enum::enum_name(mode_));
}

void AppStateLoader::save(
    const UIState& ui, const DocumentStorage& documents, const std::filesystem::path& state_path) {
    if (mode_ == StateMode::Standalone) {
        log::info("not saving the state in Standalone mode");
        return;
    }

    std::ofstream ofs(state_path);
    persister_->save(ui, documents, ofs);
    log::info("the state has been successfully saved to {}", state_path);
}

Status AppStateLoader::load(
    const std::vector<std::string>& paths,
    UIState& ui,
    DocumentStorage& documents,
    const std::filesystem::path& state_path) {
    auto mode_name = magic_enum::enum_name(mode_);

    if (mode_ != StateMode::Standalone && mode_ != StateMode::Clean) {
        log::info("loading the state because mode is {}", mode_name);
        std::ifstream ifs(state_path);

        if (auto result = persister_->restore(ui, documents, ifs); !result) {
            log::warn("failed to restore the state: {}; ignoring", result.error());
        } else {
            log::info("the state has been successfully restored");
        }
    } else {
        log::info("not loading the state because mode is {}", mode_name);
    }

    for (auto path : paths) {
        path = std::filesystem::absolute(utils::expandHome(path));

        if (isOpened(path, ui, documents) && mode_ == StateMode::AddOnly) {
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

    log::info("the state has been successfully restored from {}", state_path);
    return result::ok();
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppStateLoader);
