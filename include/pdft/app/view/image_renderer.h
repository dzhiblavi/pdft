#pragma once

#include "pdft/app/view/display_properties.h"

#include "pdft/image/image.h"

namespace pdft::app {

class ImageRenderer {
 public:
    virtual ~ImageRenderer() = default;

    virtual void render(
        platform::trace::Context& ctx,
        const DisplayProperties& display,
        image::Image& image) const = 0;
};

}  // namespace pdft::app
