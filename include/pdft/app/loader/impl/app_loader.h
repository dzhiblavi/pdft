#pragma once

#include "pdft/app/loader/loader.h"
#include "pdft/app/loader/state_loader.h"
#include "pdft/app/state/palette_storage.h"

#include "pdft/notify/notify.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include "pdft/utils/cli/cli.h"

namespace pdft::app {

class AppLoader : public Loader, public Module {
 public:
    struct CliArgs {
        cli::cli<std::vector<std::string>, 'p', "path", "Paths to the documents to be opened"> path;
    };

    AppLoader(
        CliArgs cli,
        inject<StateLoader, "state_loader"> state_loader,
        inject<notify::Notify, "fswatcher"> fswatcher,
        inject<PaletteStorage, "palettes"> palettes);

    Status load(UIState& ui, DocumentStorage& documents) override;
    Status save(const UIState& ui, const DocumentStorage& documents) override;

 private:
    CliArgs cli_;
    Arc<StateLoader> state_loader_;
    Arc<notify::Notify> fs_watcher_;
    Arc<PaletteStorage> palettes_;
};

}  // namespace pdft::app
