#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include "pdft/core/result.h"

#include <filesystem>

namespace pdft::app {

class Loader {
 public:
    virtual ~Loader() = default;

    virtual void save(
        const UIState& ui,
        const DocumentStorage& documents,
        const std::filesystem::path& state_path) = 0;

    virtual Status load(
        const std::vector<std::string>& paths,
        UIState& ui,
        DocumentStorage& documents,
        const std::filesystem::path& state_path) = 0;
};

}  // namespace pdft::app
