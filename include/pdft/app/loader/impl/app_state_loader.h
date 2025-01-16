#pragma once

#include "pdft/app/loader/persister.h"
#include "pdft/app/loader/state_loader.h"
#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui.h"

#include "pdft/media/loader.h"

#include "platform/module/module.h"

#include "platform/cli/cli.h"

namespace pdft::app {

enum class StateMode {
    Standalone,  // Ignore the state completely
    Clean,       // Clear the state and save the new one
    AddOnly,     // Add paths that are not currently opened in the state
    AddAll,      // Use the state and append all paths to it (duplicates can be created)
};

class AppStateLoader : public StateLoader, public platform::rt::Module {
 public:
    struct Config {
        StateMode mode = StateMode::AddOnly;
        std::optional<std::string> state_path = {};
    };

    struct CliArgs {
        platform::rt::cli_opt<std::optional<StateMode>>              //
            ::desc<"Override the configured state management mode">  //
            ::shrt<'S'> state_mode;
    };

    AppStateLoader(
        Config config,
        CliArgs cli,
        platform::Arc<UI> ui,
        platform::Arc<DocumentStorage> documents,
        platform::Arc<const Persister> persister,
        platform::Arc<const media::Loader> doc_loader);

    platform::Status save() override;
    platform::Status load(const std::vector<std::string>& paths) override;

 private:
    Config config_;
    platform::Arc<UI> ui_;
    platform::Arc<DocumentStorage> documents_;
    platform::Arc<const Persister> persister_;
    platform::Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
