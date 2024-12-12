#include "pdft/app/loader/impl/app_loader.h"
#include "pdft/app/palettes.h"

#include "pdft/core/module/register.h"
#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"

#include "pdft/utils/cli/register.h"

namespace pdft::app {

using namespace result;

namespace {}  // namespace

AppLoader::AppLoader(
    CliArgs cli,
    inject<StateLoader, "state_loader"> state_loader,
    inject<notify::Notify, "fswatcher"> fswatcher,
    inject<PaletteStorage, "palettes"> palettes)
    : cli_(cli)
    , state_loader_(std::move(state_loader))
    , fs_watcher_(std::move(fswatcher))
    , palettes_(std::move(palettes)) {}

Status AppLoader::load(UIState& ui, DocumentStorage& documents) {
    palettes_->add("skim", getSkimPalette());
    co_await state_loader_->load(cli_.path, ui, documents);

    for (auto&& doc : ui.states) {
        auto document = documents.getDocument(doc.document_id) | result::unwrap;
        const auto& path = document->getPath();

        log::info(this, "adding path {} to filesystem watching", path);
        fs_watcher_->add(path);
    }
}

Status AppLoader::save(const UIState& ui, const DocumentStorage& documents) {
    state_loader_->save(ui, documents);
    return ok();
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppLoader);
PDFT_REGISTER_CLI_ARGS(pdft::app::AppLoader::CliArgs);
