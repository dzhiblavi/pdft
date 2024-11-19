#pragma once

#include "pdft/view/image/kitty/display_base.h"

namespace pdft::view::image::kitty {

class UnicodeDisplay : public DisplayBase {
 public:
    using DisplayBase::DisplayBase;

    void displayImage(ImageId id, IRect rect, core::io::OutBuffer& buffer) override;
};

}  // namespace pdft::view::image::kitty
