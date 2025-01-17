#pragma once

#include "pdft/image/size.h"
#include "pdft/tui/bufpair.h"
#include "pdft/tui/rect.h"

#include "platform/trace/context.h"

namespace pdft::tui {

using ImageId = uint32_t;

class Display {
 public:
    virtual ~Display() = default;

    virtual void loadCompressedImage(
        platform::trace::Context& ctx,
        ImageId id,
        image::ImageSize size,
        std::string_view compressed_data,
        io::OutBuffer& buffer) = 0;

    virtual void displayImage(
        platform::trace::Context& ctx,
        ImageId id,
        const IRect& rect,
        const FRect& subarea,
        BufPair& buffers) = 0;

    virtual void clear(platform::trace::Context& ctx, io::OutBuffer& buffer) = 0;
    virtual void unload(platform::trace::Context& ctx, ImageId id, io::OutBuffer& buffer) = 0;
    virtual void unloadAll(platform::trace::Context& ctx, io::OutBuffer& buffer) = 0;

    virtual bool isLoaded(ImageId id) const = 0;
};

}  // namespace pdft::tui
