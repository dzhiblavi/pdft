#pragma once

#include "pdft/tui/context.h"
#include "pdft/tui/rect.h"

#include "pdft/core/io/buffer.h"
#include "pdft/image/image.h"

namespace pdft::tui::display {

class Display {
 public:
    virtual ~Display() = default;

    virtual void loadImage(
        pdft::image::ImageId id, const pdft::image::Image& image, core::io::OutBuffer& buffer) = 0;

    virtual void displayImage(
        pdft::image::ImageId id, const IRect& rect, const FRect& image_subarea, Context& ctx) = 0;

    virtual void clear(core::io::OutBuffer& buffer) = 0;

    virtual void unload(pdft::image::ImageId id, core::io::OutBuffer& buffer) = 0;

    virtual void unloadAll(core::io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(pdft::image::ImageId id) const = 0;
};

}  // namespace pdft::tui::display
