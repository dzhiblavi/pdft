#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "platform/core/async.h"
#include "platform/core/result.h"
#include "platform/trace/context.h"

#include <ftxui/screen/screen.hpp>

namespace pdft::tui {

class Renderer {
 public:
    virtual ~Renderer() = default;
    virtual platform::Async<platform::Status> initScreen(platform::trace::Context& ctx) = 0;
    virtual platform::Async<platform::Status> restoreScreen(platform::trace::Context& ctx) = 0;

    virtual Screen newFrame(platform::trace::Context& ctx, tui::Element dom) = 0;
    virtual platform::Async<platform::Status> renderFrame(platform::trace::Context& ctx) = 0;
};

}  // namespace pdft::tui
