#pragma once

#include "pdft/app/state_loader/state_loader.h"
#include "pdft/app/persist/persister.h"
#include "pdft/media/loader.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

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
    };

    AppStateLoader(
        Config config,
        std::span<const char*> argv,
        inject<const Persister, "persister"> persister,
        inject<const media::Loader, "media_loader"> doc_loader);

    void save(
        const UIState& ui,
        const DocumentStorage& documents,
        const std::filesystem::path& state_path) override;

    Status load(
        const std::vector<std::string>& paths,
        UIState& ui,
        DocumentStorage& documents,
        const std::filesystem::path& state_path) override;

 private:
    StateMode mode_;
    Arc<const Persister> persister_;
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
