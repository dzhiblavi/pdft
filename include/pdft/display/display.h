#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/media/image.h"
#include "pdft/view/rect.h"

#include <asio/awaitable.hpp>

namespace pdft::display {

class Display {
 public:
    virtual ~Display() = default;

    virtual asio::awaitable<void> loadImage(
        media::ImageId id, const media::Image& image, core::io::OutBuffer& buffer) = 0;

    virtual asio::awaitable<void> displayImage(
        media::ImageId id,
        const view::IRect& rect,
        const view::FRect& image_subarea,
        core::io::OutBuffer& buffer) = 0;

    virtual void clear(core::io::OutBuffer& buffer) = 0;

    virtual void unload(media::ImageId id, core::io::OutBuffer& buffer) = 0;

    virtual void unloadAll(core::io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(media::ImageId id) const = 0;
};

}  // namespace pdft::display
