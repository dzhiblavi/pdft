#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/tui/context.h"
#include "pdft/tui/image/fit_image.h"
#include "pdft/tui/rect.h"

namespace pdft::view {

struct Page {
    state::DocumentId document_id;
    uint32_t page_index;
};

struct PageProperties {
    const tui::IRect& rect;
    const tui::FRect& subarea = {0.f, 0.f, 1.f, 1.f};
    const tui::image::Placement& placement = {};
    const image::ImageProperties& properties = {};
};

class PageRenderer {
 public:
    virtual ~PageRenderer() = default;

    virtual void render(const Page& page, const PageProperties& properties, tui::Context& ctx) = 0;
};

}  // namespace pdft::view
