#include "pdft/app/loader/impl/app_loader.h"
#include "pdft/app/palettes.h"

#include "pdft/core/log/common_adapters.h"  // IWYU pragma: keep

#include "platform/cli/register.h"
#include "platform/core/result.h"
#include "platform/module/register.h"

namespace pdft::app {

using namespace result;
using namespace platform;

AppLoader::AppLoader(
    CliArgs cli,
    platform::Arc<UI> ui,
    platform::Arc<DocumentStorage> documents,
    platform::Arc<StateLoader> state_loader,
    platform::Arc<notify::Notify> fswatcher,
    platform::Arc<PaletteStorage> palettes)
    : cli_(cli)
    , ui_(std::move(ui))
    , documents_(std::move(documents))
    , state_loader_(std::move(state_loader))
    , fs_watcher_(std::move(fswatcher))
    , palettes_(std::move(palettes)) {}

platform::Status AppLoader::load() {
    palettes_->add("skim", getSkimPalette());
    co_await state_loader_->load(cli_.path);

    for (auto&& doc : ui_->getModel().documents) {
        auto document = documents_->getDocument(doc.document_id) | result::unwrap;
        const auto& path = document->getPath();

        log::info(this, "adding path {} to filesystem watching", path);
        fs_watcher_->add(path);
    }

    co_return unit;
}

platform::Status AppLoader::save() {
    return state_loader_->save();
}

}  // namespace pdft::app

PLATFORM_REGISTER_MODULE(pdft::app::AppLoader);
PLATFORM_REGISTER_CLI_ARGS(pdft::app::AppLoader::CliArgs);
