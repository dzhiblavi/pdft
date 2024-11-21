#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/media/image.h"
#include "pdft/view/rect.h"

namespace pdft::view::image {

class Display {
 public:
    virtual ~Display() = default;

    virtual void loadImage(
        media::ImageId id, const media::Image& image, core::io::OutBuffer& buffer) = 0;

    virtual void displayImage(
        media::ImageId id,
        const IRect& rect,
        const IRect& image_size,
        core::io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(media::ImageId id) const = 0;
    virtual void clear(core::io::OutBuffer& buffer) const = 0;
};

}  // namespace pdft::view::image
