#pragma once

#include "pdft/app/loader/persister.h"
#include "pdft/app/loader/state_loader.h"
#include "pdft/media/loader.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include "pdft/utils/cli/cli.h"

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
        StateMode mode;
        std::optional<std::string> state_path;
    };

    struct CliArgs {
        cli::cli<
            std::optional<StateMode>,
            'S',
            "state-mode",
            "Override the configured state management mode">
            state_mode;
    };

    AppStateLoader(
        Config config,
        CliArgs cli,
        inject<const Persister, "persister"> persister,
        inject<const media::Loader, "media_loader"> doc_loader);

    void save(const UIState& ui, const DocumentStorage& documents) override;

    Status load(
        const std::vector<std::string>& paths, UIState& ui, DocumentStorage& documents) override;

 private:
    Config config_;
    Arc<const Persister> persister_;
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
