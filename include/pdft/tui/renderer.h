#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "platform/core/async.h"
#include "pdft/core/context.h"
#include "platform/core/result.h"

#include <ftxui/screen/screen.hpp>

namespace pdft::tui {

class Renderer {
 public:
    virtual ~Renderer() = default;
    virtual platform::Async<platform::Status> initScreen(Context& ctx) = 0;
    virtual platform::Async<platform::Status> restoreScreen(Context& ctx) = 0;

    virtual Screen newFrame(Context& ctx, tui::Element dom) = 0;
    virtual platform::Async<platform::Status> renderFrame(Context& ctx) = 0;
};

}  // namespace pdft::tui
