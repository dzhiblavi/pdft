#pragma once

#include "pdft/app/state/ui_state.h"

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

namespace pdft::app::view {

class View {
 public:
    virtual ~View() = default;

    virtual tui::Element render(Context& ctx, const state::UIState& ui, tui::Screen& screen) = 0;
};

}  // namespace pdft::app::view
