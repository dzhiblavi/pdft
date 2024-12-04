#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include "pdft/core/result.h"

#include <ostream>

namespace pdft::app::persist {

enum class PersisterError {
    FailedToReadState,
};

class Persister {
 public:
    virtual ~Persister() = default;

    virtual void save(
        const state::UIState& ui,
        const state::DocumentStorage& documents,
        std::ostream& os) const = 0;

    virtual Result<void> restore(
        state::UIState& ui, state::DocumentStorage& documents, std::istream& is) const = 0;
};

}  // namespace pdft::app::persist
