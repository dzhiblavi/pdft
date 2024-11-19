#include "pdft/view/image/kitty/unicode_display.h"
#include "pdft/core/log/log.h"

#include "./detail/rowcolumn_diacritics.h"

namespace pdft::view::image::kitty {

void UnicodeDisplay::displayImage(ImageId id, IRect rect, core::io::OutBuffer& buffer) {
    auto term_size = core::terminal::getTerminalDimensions();

    const auto& [x, y, w, h] = rect;
    core::log::global()->info("display @ x={},y={},w={},h={}", x, y, w, h);

    // x, y, w, h (pixels) control the subrectangle of the image (not in unicode mode btw).
    // In case of Unicode mode (U=1) the image subrectangle is controlled via placing
    // the correct row and column indices.
    //
    // c, r (columns and rows) control the rectangle at which the image will be displayed
    // The rectangle position in achieved via moving the cursor to the top-left corner
    // before displaying the image.
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},
        {"U", "1"},
        {"i", std::to_string(id)},
        {"r", std::to_string(h)},
        {"c", std::to_string(w)},
        {"C", "1"},
    };

    // Create a virtual PagePlacement
    executor_->execute(
        {
            .command = "_G",
            .payload_b64 = "",
            .control_data = control_data,
        },
        buffer);

    uint32_t left = static_cast<uint32_t>(std::max(0, x));
    uint32_t top = static_cast<uint32_t>(std::max(0, y));
    uint32_t num_rows = h + std::min(0, y);
    uint32_t num_cols = w + std::min(0, x);

    // Fill the image space with special unicode characters
    const std::string_view ImagePlaceholder = "\U0010EEEE";
    const std::string_view IdChar = detail::RowColDiacritics[(id >> 24) & 255];

    core::io::write(
        buffer, "\033[38:2:", (id >> 16) & 255, ":", (id >> 8) & 255, ":", (id >> 0) & 255, "m");

    for (uint32_t row = top; row < top + num_rows; ++row) {
        terminal_->moveCursor(
            core::terminal::CursorPosition{
                .rows = row + 1,
                .columns = left + 1,
            },
            buffer);

        for (uint32_t col = left; col < left + num_cols; ++col) {
            core::io::write(
                buffer,
                ImagePlaceholder,
                detail::RowColDiacritics[row - y],
                detail::RowColDiacritics[col - x],
                IdChar);
        }
    }

    core::io::write(buffer, "\033[39m");
}

}  // namespace pdft::view::image::kitty
