#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "platform/core/async.h"
#include "platform/trace/context.h"

namespace pdft::app {

class View {
 public:
    virtual ~View() = default;

    virtual tui::Element getLayout(platform::trace::Context& ctx) = 0;
    virtual platform::Async<void> render(platform::trace::Context& ctx, tui::Screen& screen) = 0;
};

}  // namespace pdft::app
