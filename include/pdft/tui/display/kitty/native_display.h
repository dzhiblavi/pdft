#pragma once

#include "pdft/tui/display/kitty/display_base.h"

namespace pdft::tui::display::kitty {

class NativeDisplay : public DisplayBase {
 public:
    using DisplayBase::DisplayBase;

    void displayImage(
        image::ImageId id, const IRect& rect, const FRect& image_subarea, Context& ctx) override;
};

}  // namespace pdft::tui::display::kitty
