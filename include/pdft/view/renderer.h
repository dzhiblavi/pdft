#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/state/document_storage.h"
#include "pdft/view/placement.h"
#include "pdft/view/rect.h"

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
        core::io::OutBuffer& buffer,
        const IRect& rect,
        const PagePlacement& placement,
        const display::ImageProperties& properties,
        const Page& page) = 0;

    virtual void render(
        core::io::OutBuffer& buffer,
        const IRect& rect,
        const TextPlacement& placement,
        std::string_view text) = 0;
};

}  // namespace pdft::view
