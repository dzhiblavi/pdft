#include "pdft/app/loader/impl/app_state_loader.h"

#include "pdft/app/state/ui.h"
#include "pdft/core/log.h"
#include "pdft/runtime/module/register.h"
#include "pdft/core/result.h"

#include "pdft/runtime/cli/register.h"
#include "pdft/utils/get_home_dir.h"

#include <magic_enum.hpp>

namespace pdft::app {

namespace {

bool isOpened(const std::string& abs_path, const UIModel& ui, const DocumentStorage& documents) {
    // XXX this does not actually require linear search

    for (const auto& state : ui.documents) {
        auto doc = documents.getDocument(state.document_id) | result::unwrap;

        if (abs_path == doc->getPath()) {
            return true;
        }
    }

    return false;
}

}  // namespace

AppStateLoader::AppStateLoader(
    Config config,
    CliArgs cli,
    Arc<UI> ui,
    Arc<DocumentStorage> documents,
    Arc<const Persister> persister,
    Arc<const media::Loader> doc_loader)
    : config_(config)
    , ui_(std::move(ui))
    , documents_(std::move(documents))
    , persister_(std::move(persister))
    , doc_loader_(std::move(doc_loader)) {
    config_.mode = cli.state_mode->value_or(config_.mode);
    config_.state_path = config_.state_path.transform(utils::expandHome);
    log::info(this, "AppStateLoader state mode is {}", magic_enum::enum_name(config_.mode));
}

Status AppStateLoader::save() {
    if (config_.mode == StateMode::Standalone) {
        log::info(this, "not saving the state in Standalone mode");
        return result::ok();
    }
    if (!config_.state_path.has_value()) {
        log::info(this, "not saving the state because no state path is configured");
        return result::ok();
    }

    std::ofstream ofs(*config_.state_path);
    log::info(this, "saving state to {}", *config_.state_path);
    return persister_->save(ofs);
}

Status AppStateLoader::load(const std::vector<std::string>& paths) {
    if (!config_.state_path.has_value()) {
        log::info(this, "not loading the state because state path is not configured");
        return result::ok();
    }

    auto mode_name = magic_enum::enum_name(config_.mode);

    if (config_.mode != StateMode::Standalone && config_.mode != StateMode::Clean) {
        log::info(this, "loading the state because mode is {}", mode_name);
        std::ifstream ifs(*config_.state_path);

        if (auto result = persister_->restore(ifs); !result) {
            log::warn(this, "failed to restore the state: {}; ignoring", result.error());
        } else {
            log::info(this, "the state has been successfully restored");
        }
    } else {
        log::info(this, "not loading the state because mode is {}", mode_name);
    }

    for (auto path : paths) {
        path = std::filesystem::absolute(utils::expandHome(path));

        if (isOpened(path, ui_->getModel(), *documents_) && config_.mode == StateMode::AddOnly) {
            log::info(this, "ignoring the new path {} because state mode is AddOnly", path);
            continue;
        }

        auto maybe_document = doc_loader_->load(path);
        if (!maybe_document) {
            log::warn(this, "failed to open document by path {}: {}", path, maybe_document.error());
            continue;
        }

        auto document = *maybe_document;
        auto page_count = document->getPageCount();
        auto doc_id = documents_->addDocument(document);
        ui_->addDocument(doc_id, page_count);
        log::info(this, "loaded path {}", path);
    }

    log::info(this, "the state has been successfully restored from {}", *config_.state_path);
    return result::ok();
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppStateLoader);
PDFT_REGISTER_CLI_ARGS(pdft::app::AppStateLoader::CliArgs);
