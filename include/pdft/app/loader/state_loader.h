#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include "pdft/core/result.h"

namespace pdft::app {

class StateLoader {
 public:
    virtual ~StateLoader() = default;

    virtual void save(const UIState& ui, const DocumentStorage& documents) = 0;

    virtual Status load(
        const std::vector<std::string>& paths, UIState& ui, DocumentStorage& documents) = 0;
};

}  // namespace pdft::app
