#pragma once

#include "pdft/app/loader/loader.h"

namespace pdft::app::loader::impl {

class Loader : public loader::Loader {
 public:
    void save(const state::UIState& ui, const state::DocumentStorage& documents, std::ostream& os)
        override;

    Result<void> restore(
        state::UIState& ui, state::DocumentStorage& documents, std::istream& is) override;
};

}  // namespace pdft::app::loader::impl
