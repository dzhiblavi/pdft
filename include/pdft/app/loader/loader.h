#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include "pdft/core/result/type/status.h"

namespace pdft::app {

class Loader {
 public:
    virtual ~Loader() = default;

    virtual Status load(UIState& ui, DocumentStorage& documents) = 0;
    virtual Status save(const UIState& ui, const DocumentStorage& documents) = 0;
};

}  // namespace pdft::app
