#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/tui/context.h"
#include "pdft/tui/image/fit_image.h"
#include "pdft/tui/layout/rect.h"

#include <asio/awaitable.hpp>

namespace pdft::view {

struct Page {
    state::DocumentId document_id;
    uint32_t page_index;
};

class Renderer {
 public:
    virtual ~Renderer() = default;

    virtual asio::awaitable<void> render(
        const tui::layout::IRect& rect,
        const tui::image::Placement& placement,
        const image::ImageProperties& properties,
        const Page& page,
        tui::Context& ctx) = 0;
};

}  // namespace pdft::view
