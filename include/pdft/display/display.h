#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/display/image.h"
#include "pdft/view/rect.h"

#include <asio/awaitable.hpp>

namespace pdft::display {

class Display {
 public:
    virtual ~Display() = default;

    virtual asio::awaitable<void> loadImage(
        display::ImageId id, const display::Image& image, core::io::OutBuffer& buffer) = 0;

    virtual asio::awaitable<void> displayImage(
        display::ImageId id,
        const view::IRect& rect,
        const view::FRect& image_subarea,
        core::io::OutBuffer& buffer) = 0;

    virtual void clear(core::io::OutBuffer& buffer) = 0;

    virtual void unload(display::ImageId id, core::io::OutBuffer& buffer) = 0;

    virtual void unloadAll(core::io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(display::ImageId id) const = 0;
};

}  // namespace pdft::display
