#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/context.h"
#include "pdft/core/result.h"

#include <ftxui/screen/screen.hpp>

namespace pdft::tui {

class Renderer {
 public:
    virtual ~Renderer() = default;
    virtual Async<Status> initScreen(Context& ctx) = 0;
    virtual Async<Status> restoreScreen(Context& ctx) = 0;

    virtual Screen newFrame(Context& ctx, tui::Element dom) = 0;
    virtual Async<Status> renderFrame(Context& ctx) = 0;
};

}  // namespace pdft::tui
