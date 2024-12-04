#pragma once

#include "pdft/app/state/ui_state.h"

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "pdft/core/context.h"

namespace pdft::app {

class View {
 public:
    virtual ~View() = default;

    virtual tui::Element render(Context& ctx, const UIState& ui, tui::Screen& screen) = 0;
};

}  // namespace pdft::app
