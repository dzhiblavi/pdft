#pragma once

#include "pdft/app/view/display_properties.h"

#include "pdft/tui/fit_image.h"
#include "pdft/tui/rect.h"
#include "pdft/tui/screen.h"

#include "pdft/core/context.h"

namespace pdft::app {

struct PagePosition {
    const tui::IRect& rect;
    const tui::FRect& subarea = {0.f, 0.f, 1.f, 1.f};
    const tui::Placement& placement = {};
};

class PageRenderer {
 public:
    virtual ~PageRenderer() = default;

    virtual void render(
        Context& ctx,
        const Page& page,
        const PagePosition& pos,
        const DisplayProperties& props,
        tui::Screen& screen) = 0;
};

}  // namespace pdft::app

PDFT_LOG_ADAPT_TYPE_JSON(pdft::app, Page);
