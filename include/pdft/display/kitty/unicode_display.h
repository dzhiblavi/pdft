#pragma once

#include "pdft/core/terminal/terminal.h"
#include "pdft/display/kitty/display_base.h"

namespace pdft::display::kitty {

class UnicodeDisplay : public DisplayBase {
 public:
    using DisplayBase::DisplayBase;

    void displayImage(
        media::ImageId id,
        const view::IRect& rect,
        const view::FRect& image_subarea,
        core::io::OutBuffer& buffer) override;

 private:
    core::terminal::Dimensions term_size_ = core::terminal::getTerminalDimensions();
};

}  // namespace pdft::display::kitty
