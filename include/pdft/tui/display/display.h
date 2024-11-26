#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/image/image.h"
#include "pdft/tui/context.h"
#include "pdft/tui/rect.h"

namespace pdft::tui::display {

class Display {
 public:
    virtual ~Display() = default;

    virtual void loadImage(
        image::ImageId id, const image::Image& image, core::io::OutBuffer& buffer) = 0;

    virtual void displayImage(
        image::ImageId id, const IRect& rect, const FRect& image_subarea, Context& ctx) = 0;

    virtual void clear(core::io::OutBuffer& buffer) = 0;

    virtual void unload(image::ImageId id, core::io::OutBuffer& buffer) = 0;

    virtual void unloadAll(core::io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(image::ImageId id) const = 0;
};

}  // namespace pdft::tui::display
