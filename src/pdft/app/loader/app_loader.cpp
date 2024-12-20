#include "pdft/app/loader/impl/app_loader.h"
#include "pdft/app/palettes.h"

#include "pdft/core/module/register.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep

#include "pdft/core/cli/register.h"

namespace pdft::app {

using namespace result;

AppLoader::AppLoader(
    CliArgs cli,
    Arc<UI> ui,
    Arc<DocumentStorage> documents,
    Arc<StateLoader> state_loader,
    Arc<notify::Notify> fswatcher,
    Arc<PaletteStorage> palettes)
    : cli_(cli)
    , ui_(std::move(ui))
    , documents_(std::move(documents))
    , state_loader_(std::move(state_loader))
    , fs_watcher_(std::move(fswatcher))
    , palettes_(std::move(palettes)) {}

Status AppLoader::load() {
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

Status AppLoader::save() {
    return state_loader_->save();
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppLoader);
PDFT_REGISTER_CLI_ARGS(pdft::app::AppLoader::CliArgs);
