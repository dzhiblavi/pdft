#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/media/image.h"
#include "pdft/view/rect.h"

namespace pdft::view::image {

using ImageId = uint32_t;

class Display {
 public:
    virtual ~Display() = default;

    virtual void loadImage(ImageId id, const media::Image& image, core::io::OutBuffer& buffer) = 0;
    virtual void displayImage(ImageId id, IRect rect, core::io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(ImageId id) const = 0;
    virtual void clear(core::io::OutBuffer& buffer) const = 0;
};

}  // namespace pdft::view::image
