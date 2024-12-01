#pragma once

#include "pdft/app/state/ui_state.h"
#include "pdft/tui/context.h"
#include "pdft/tui/elements.h"

namespace pdft::app::view {

class View {
 public:
    virtual ~View() = default;

    virtual tui::Element render(const state::UIState& ui, tui::Context& ctx) = 0;
};

}  // namespace pdft::app::view
