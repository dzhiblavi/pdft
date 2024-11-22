#pragma once

#include "pdft/display/kitty/display_base.h"

namespace pdft::display::kitty {

class UnicodeDisplay : public DisplayBase {
 public:
    using DisplayBase::DisplayBase;

    void displayImage(
        media::ImageId id,
        const view::IRect& rect,
        const view::IRect& image,
        core::io::OutBuffer& buffer) override;
};

}  // namespace pdft::display::kitty
