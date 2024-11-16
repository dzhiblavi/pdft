#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/display/position.h"
#include "pdft/media/image.h"

namespace pdft::display {

using ImageId = uint32_t;

class Display {
 public:
    virtual ~Display() = default;

    virtual bool isLoaded(ImageId id) const = 0;
    virtual void loadImage(ImageId id, const media::Image& image, core::OutBuffer& buffer) = 0;
    virtual void displayImage(ImageId id, Position position, core::OutBuffer& buffer) = 0;
    virtual void clear(core::OutBuffer& buffer) const = 0;
};

}  // namespace pdft::display
