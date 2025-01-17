#pragma once

#include "pdft/app/view/display_properties.h"

#include "pdft/tui/fit_image.h"
#include "pdft/tui/rect.h"
#include "pdft/tui/screen.h"

#include "platform/core/async.h"
#include "platform/trace/context.h"

namespace pdft::app {

struct PagePosition {
    const tui::IRect& rect;
    const tui::FRect& subarea = {0.f, 0.f, 1.f, 1.f};
    const tui::Placement& placement = {};
};

class PageRenderer {
 public:
    virtual ~PageRenderer() = default;

    virtual platform::Async<platform::Unit> render(
        platform::trace::Context& ctx,
        const Page& page,
        const PagePosition& pos,
        const DisplayProperties& props,
        tui::Screen& screen) = 0;
};

}  // namespace pdft::app
