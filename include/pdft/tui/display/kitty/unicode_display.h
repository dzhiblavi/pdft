#pragma once

#include "pdft/tui/display/kitty/display_base.h"

namespace pdft::tui::display::kitty {

class UnicodeDisplay : public DisplayBase {
 public:
    using DisplayBase::DisplayBase;

    asio::awaitable<void> displayImage(
        image::ImageId id,
        const layout::IRect& rect,
        const layout::FRect& image_subarea,
        ContextRef ctx) override;
};

}  // namespace pdft::tui::display::kitty
