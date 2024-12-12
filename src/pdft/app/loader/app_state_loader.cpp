#include "pdft/app/loader/impl/app_state_loader.h"

#include "pdft/core/log.h"
#include "pdft/core/module/register.h"
#include "pdft/core/result.h"

#include "pdft/utils/cli/parse.h"
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
    cli::cli<
        std::optional<StateMode>,
        'S',
        "state-mode",
        "Override the configured state management mode">
        state_mode;
};

}  // namespace

AppStateLoader::AppStateLoader(
    Config config,
    std::span<const char*> argv,
    inject<const Persister, "persister"> persister,
    inject<const media::Loader, "media_loader"> doc_loader)
    : config_(config)
    , persister_(std::move(persister))
    , doc_loader_(std::move(doc_loader)) {
    auto cli = cli::parseFromCliArgs<CliArgs>(argv) | result::unwrap;
    config_.mode = cli.state_mode->value_or(config_.mode);
    config_.state_path = config_.state_path.transform(utils::expandHome);
    log::info(this, "AppStateLoader state mode is {}", magic_enum::enum_name(config_.mode));
}

void AppStateLoader::save(const UIState& ui, const DocumentStorage& documents) {
    if (config_.mode == StateMode::Standalone) {
        log::info("not saving the state in Standalone mode");
        return;
    }
    if (!config_.state_path.has_value()) {
        log::info("not saving the state because no state path is configured");
        return;
    }

    std::ofstream ofs(*config_.state_path);
    persister_->save(ui, documents, ofs);
    log::info("the state has been successfully saved to {}", *config_.state_path);
}

Status AppStateLoader::load(
    const std::vector<std::string>& paths, UIState& ui, DocumentStorage& documents) {
    if (!config_.state_path.has_value()) {
        log::info(this, "not loading the state because state path is not configured");
        return result::ok();
    }

    auto mode_name = magic_enum::enum_name(config_.mode);

    if (config_.mode != StateMode::Standalone && config_.mode != StateMode::Clean) {
        log::info("loading the state because mode is {}", mode_name);
        std::ifstream ifs(*config_.state_path);

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

        if (isOpened(path, ui, documents) && config_.mode == StateMode::AddOnly) {
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

    log::info("the state has been successfully restored from {}", *config_.state_path);
    return result::ok();
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppStateLoader);
