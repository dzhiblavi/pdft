#pragma once

#include "pdft/display/kitty/display_base.h"

namespace pdft::display::kitty {

class NativeDisplay : public DisplayBase {
 public:
    using DisplayBase::DisplayBase;

    asio::awaitable<void> displayImage(
        media::ImageId id,
        const view::IRect& rect,
        const view::FRect& image_subarea,
        core::io::OutBuffer& buffer) override;
};

}  // namespace pdft::display::kitty
