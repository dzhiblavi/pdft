#pragma once

#include "pdft/state/state.h"
#include "pdft/tui/context.h"
#include "pdft/tui/elements.h"

namespace pdft::view {

class View {
 public:
    virtual ~View() = default;
    virtual tui::Element render(const state::State& state, tui::Context& ctx) = 0;
};

}  // namespace pdft::view
