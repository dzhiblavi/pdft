#pragma once

#include "pdft/app/state/document_storage.h"

#include "pdft/tui/image/fit_image.h"
#include "pdft/tui/rect.h"
#include "pdft/tui/screen.h"

#include "pdft/image/palette.h"

#include "pdft/core/context.h"

namespace pdft::app::view {

struct Page {
    state::DocumentId document_id;
    uint32_t page_index;
};

struct PagePosition {
    const tui::IRect& rect;
    const tui::FRect& subarea = {0.f, 0.f, 1.f, 1.f};
    const tui::image::Placement& placement = {};
};

struct DisplayProperties {
    float scaling_coeff = 1.f;
    bool invert_colors = false;
    std::optional<image::PaletteRef> palette = std::nullopt;
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

}  // namespace pdft::app::view

PDFT_LOG_ADAPT_TYPE_JSON(pdft::app::view, Page);
