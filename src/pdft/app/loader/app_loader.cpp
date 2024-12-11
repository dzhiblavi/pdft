#include "pdft/app/loader/impl/app_loader.h"
#include "pdft/app/palettes.h"

#include "pdft/core/module/register.h"
#include "pdft/utils/cli_args/cli_args.h"

#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"

namespace pdft::app {

using namespace result;

namespace {

struct CliArgs {
    std::vector<std::string> path;
};

}  // namespace

AppLoader::AppLoader(
    std::span<const char*> argv,
    inject<StateLoader, "state_loader"> state_loader,
    inject<notify::Notify, "fswatcher"> fswatcher,
    inject<PaletteStorage, "palettes"> palettes)
    : argv_(argv)
    , state_loader_(std::move(state_loader))
    , fs_watcher_(std::move(fswatcher))
    , palettes_(std::move(palettes)) {}

Status AppLoader::load(UIState& ui, DocumentStorage& documents) {
    palettes_->add("skim", getSkimPalette());

    auto args = co_await utils::parseFromCliArgs<CliArgs>(argv_);
    co_await state_loader_->load(args.path, ui, documents);

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
