#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include "pdft/media/loader.h"

#include "pdft/core/result.h"

#include <ostream>

namespace pdft::app::loader {

class Loader {
 public:
    virtual ~Loader() = default;

    virtual void save(
        const state::UIState& ui, const state::DocumentStorage& documents, std::ostream& os) = 0;

    virtual Result<void> restore(
        state::UIState& ui,
        state::DocumentStorage& documents,
        const media::Loader& loader,
        std::istream& is) = 0;
};

}  // namespace pdft::app::loader
