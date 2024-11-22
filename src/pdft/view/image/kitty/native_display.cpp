#include "pdft/view/image/kitty/native_display.h"
#include "pdft/core/log/log.h"

namespace pdft::view::image::kitty {

void NativeDisplay::displayImage(
    media::ImageId id, const IRect& rect, const IRect&, core::io::OutBuffer& buffer) {
    const auto& [x, y, w, h] = rect;
    core::log::global()->info("display @ x={},y={},w={},h={}", x, y, w, h);

    uint32_t left = static_cast<uint32_t>(std::max(0, x));
    uint32_t top = static_cast<uint32_t>(std::max(0, y));

    core::log::global()->info("DISPLAY r={}, c={}", h + std::min(0, y), w + std::min(0, x));

    // x, y, w, h (pixels) control the subrectangle of the image
    // c, r (columns and rows) control the rectangle at which the image will be displayed
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},
        {"i", std::to_string(id)},
        {"r", std::to_string(h + std::min(0, y))},
        //{"c", std::to_string(w + std::min(0, x))},
        //{"x", "20"},
        //{"y", "20"},
        //{"w", "20"},
        //{"h", "20"},
        {"C", "1"},
    };

    // Position cursor to the top left corner
    terminal_->moveCursor(
        core::terminal::CursorPosition{
            .rows = top,
            .columns = left,
        },
        buffer);

    // Create a virtual PagePlacement
    executor_->execute(
        {
            .command = "_G",
            .payload_b64 = "",
            .control_data = control_data,
        },
        buffer);
}

}  // namespace pdft::view::image::kitty
