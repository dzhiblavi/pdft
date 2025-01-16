#pragma once

#include "pdft/app/loader/loader.h"
#include "pdft/app/loader/state_loader.h"
#include "pdft/app/state/palette_storage.h"
#include "pdft/app/state/ui.h"

#include "pdft/notify/notify.h"

#include "platform/module/module.h"

#include "platform/cli/cli.h"

namespace pdft::app {

class AppLoader : public Loader, public platform::rt::Module {
 public:
    struct CliArgs {
        platform::rt::cli_opt<std::vector<std::string>>  //
            ::desc<"Paths to the documents to be opened"> path;
    };

    AppLoader(
        CliArgs cli,
        platform::Arc<UI> ui,
        platform::Arc<DocumentStorage> documents,
        platform::Arc<StateLoader> state_loader,
        platform::Arc<notify::Notify> fswatcher,
        platform::Arc<PaletteStorage> palettes);

    platform::Status load() override;
    platform::Status save() override;

 private:
    CliArgs cli_;
    platform::Arc<UI> ui_;
    platform::Arc<DocumentStorage> documents_;
    platform::Arc<StateLoader> state_loader_;
    platform::Arc<notify::Notify> fs_watcher_;
    platform::Arc<PaletteStorage> palettes_;
};

}  // namespace pdft::app
