#pragma once

#include "pdft/app/loader/loader.h"
#include "pdft/app/persist/persister.h"

#include "pdft/media/loader.h"

namespace pdft::app {

enum class StateMode {
    Standalone,  // Ignore the state completely
    Clean,       // Clear the state and save the new one
    AddOnly,     // Add paths that are not currently opened in the state
    AddAll,      // Use the state and append all paths to it (duplicates can be created)
};

class AppLoader : public Loader {
 public:
    AppLoader(StateMode mode, Arc<const Persister> persister, Arc<const media::Loader> doc_loader)
        : mode_(mode)
        , persister_(std::move(persister))
        , doc_loader_(std::move(doc_loader)) {}

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
    const StateMode mode_;
    Arc<const Persister> persister_;
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
