#pragma once

#include "pdft/view/image/kitty/display_base.h"

namespace pdft::view::image::kitty {

class NativeDisplay : public DisplayBase {
 public:
    using DisplayBase::DisplayBase;

    void displayImage(
        media::ImageId id,
        const IRect& rect,
        const IRect& image,
        core::io::OutBuffer& buffer) override;
};

}  // namespace pdft::view::image::kitty
