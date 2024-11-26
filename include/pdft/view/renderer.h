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

class Renderer {
 public:
    virtual ~Renderer() = default;

    virtual void render(
        const tui::IRect& rect,
        const tui::FRect& subarea,
        const tui::image::Placement& placement,
        const image::ImageProperties& properties,
        const Page& page,
        tui::Context& ctx) = 0;

    void render(
        const tui::IRect& rect,
        const tui::image::Placement& placement,
        const image::ImageProperties& properties,
        const Page& page,
        tui::Context& ctx) {
        auto full = tui::FRect{
            .width = 1.f,
            .height = 1.f,
        };
        render(rect, full, placement, properties, page, ctx);
    }
};

}  // namespace pdft::view
