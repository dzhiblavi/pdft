#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include "pdft/core/result.h"

#include <ostream>

namespace pdft::app::loader {

enum class StateMode {
    Standalone,  // Ignore the state completely
    Clean,       // Clear the state and save the new one
    AddOnly,     // Add paths that are not currently opened in the state
    AddAll,      // Use the state and append all paths to it (duplicates can be created)
};

inline bool shouldSave(StateMode mode) {
    switch (mode) {
        case StateMode::Standalone:
            return false;
        case StateMode::Clean:
        case StateMode::AddOnly:
        case StateMode::AddAll:
            return true;
    }
}

class Loader {
 public:
    virtual ~Loader() = default;

    virtual void save(const UIState& ui, const DocumentStorage& documents, std::ostream& os) = 0;

    virtual Result<void> load(
        const std::vector<std::string>& paths,
        UIState& ui,
        DocumentStorage& documents,
        StateMode mode,
        std::istream& is) = 0;
};

}  // namespace pdft::app::loader
