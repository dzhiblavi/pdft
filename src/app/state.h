#pragma once

namespace app {

enum class StateMode {
    Standalone,  // Ignore the state completely
    Clean,       // Clear the state and save the new one
    AddOnly,     // Add paths that are not currently opened in the state
    AddAll,      // Use the state and append all paths to it (duplicates can be created)
};

}  // namespace app
