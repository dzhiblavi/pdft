#pragma once

#include "pdft/tui/bufpair.h"
#include "pdft/tui/rect.h"

#include "pdft/image/image.h"

#include "pdft/core/context.h"

namespace pdft::tui {

using ImageId = uint32_t;

class Display {
 public:
    virtual ~Display() = default;

    virtual void loadImage(
        Context& ctx, ImageId id, const pdft::image::Image& image, io::OutBuffer& buffer) = 0;

    virtual void displayImage(
        Context& ctx, ImageId id, const IRect& rect, const FRect& subarea, BufPair& buffers) = 0;

    virtual void clear(Context& ctx, io::OutBuffer& buffer) = 0;
    virtual void unload(Context& ctx, ImageId id, io::OutBuffer& buffer) = 0;
    virtual void unloadAll(Context& ctx, io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(ImageId id) const = 0;
};

}  // namespace pdft::tui
