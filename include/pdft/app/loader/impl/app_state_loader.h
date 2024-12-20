#pragma once

#include "pdft/app/loader/persister.h"
#include "pdft/app/loader/state_loader.h"
#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui.h"

#include "pdft/media/loader.h"

#include "pdft/core/module/module.h"

#include "pdft/core/cli/cli.h"

namespace pdft::app {

enum class StateMode {
    Standalone,  // Ignore the state completely
    Clean,       // Clear the state and save the new one
    AddOnly,     // Add paths that are not currently opened in the state
    AddAll,      // Use the state and append all paths to it (duplicates can be created)
};

class AppStateLoader : public StateLoader, public Module {
 public:
    struct Config {
        StateMode mode = StateMode::AddOnly;
        std::optional<std::string> state_path = {};
    };

    struct CliArgs {
        cli::opt<std::optional<StateMode>>                           //
            ::desc<"Override the configured state management mode">  //
            ::shrt<'S'> state_mode;
    };

    AppStateLoader(
        Config config,
        CliArgs cli,
        Arc<UI> ui,
        Arc<DocumentStorage> documents,
        Arc<const Persister> persister,
        Arc<const media::Loader> doc_loader);

    Status save() override;
    Status load(const std::vector<std::string>& paths) override;

 private:
    Config config_;
    Arc<UI> ui_;
    Arc<DocumentStorage> documents_;
    Arc<const Persister> persister_;
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
