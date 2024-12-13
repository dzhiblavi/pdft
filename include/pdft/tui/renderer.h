#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/context.h"

#include <ftxui/screen/screen.hpp>

namespace pdft::tui {

class Renderer {
 public:
    virtual ~Renderer() = default;
    virtual AsyncStatus initScreen(Context& ctx) = 0;
    virtual AsyncStatus restoreScreen(Context& ctx) = 0;

    virtual Screen newFrame(Context& ctx, tui::Element dom) = 0;
    virtual AsyncStatus renderFrame(Context& ctx) = 0;
};

}  // namespace pdft::tui
