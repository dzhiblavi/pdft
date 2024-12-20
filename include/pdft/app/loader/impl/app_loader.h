#pragma once

#include "pdft/app/loader/loader.h"
#include "pdft/app/loader/state_loader.h"
#include "pdft/app/state/palette_storage.h"
#include "pdft/app/state/ui.h"

#include "pdft/notify/notify.h"

#include "pdft/core/module/module.h"

#include "pdft/core/cli/cli.h"

namespace pdft::app {

class AppLoader : public Loader, public Module {
 public:
    struct CliArgs {
        cli::opt<std::vector<std::string>>  //
            ::desc<"Paths to the documents to be opened"> path;
    };

    AppLoader(
        CliArgs cli,
        Arc<UI> ui,
        Arc<DocumentStorage> documents,
        Arc<StateLoader> state_loader,
        Arc<notify::Notify> fswatcher,
        Arc<PaletteStorage> palettes);

    Status load() override;
    Status save() override;

 private:
    CliArgs cli_;
    Arc<UI> ui_;
    Arc<DocumentStorage> documents_;
    Arc<StateLoader> state_loader_;
    Arc<notify::Notify> fs_watcher_;
    Arc<PaletteStorage> palettes_;
};

}  // namespace pdft::app
