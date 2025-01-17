#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "pdft/core/context.h"
#include "platform/core/async.h"

namespace pdft::app {

class View {
 public:
    virtual ~View() = default;

    virtual tui::Element getLayout(Context& ctx) = 0;
    virtual platform::Async<void> render(Context& ctx, tui::Screen& screen) = 0;
};

}  // namespace pdft::app
