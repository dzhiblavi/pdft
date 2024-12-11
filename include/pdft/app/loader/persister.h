#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include "pdft/core/result.h"

#include <ostream>

namespace pdft::app {

class Persister {
 public:
    virtual ~Persister() = default;

    virtual void save(
        const UIState& ui, const DocumentStorage& documents, std::ostream& os) const = 0;

    virtual Status restore(UIState& ui, DocumentStorage& documents, std::istream& is) const = 0;
};

}  // namespace pdft::app
