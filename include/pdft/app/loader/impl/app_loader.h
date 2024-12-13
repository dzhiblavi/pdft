#pragma once

#include "pdft/app/loader/loader.h"
#include "pdft/app/loader/state_loader.h"
#include "pdft/app/state/palette_storage.h"
#include "pdft/app/state/ui.h"

#include "pdft/notify/notify.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include "pdft/utils/cli/cli.h"

namespace pdft::app {

class AppLoader : public Loader, public Module {
 public:
    struct CliArgs {
        cli::cli<std::vector<std::string>, 'p', "Paths to the documents to be opened"> path;
    };

    AppLoader(
        CliArgs cli,
        inject<UI, "ui"> ui,
        inject<DocumentStorage, "docs"> documents,
        inject<StateLoader, "state_loader"> state_loader,
        inject<notify::Notify, "fswatcher"> fswatcher,
        inject<PaletteStorage, "palettes"> palettes);

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
