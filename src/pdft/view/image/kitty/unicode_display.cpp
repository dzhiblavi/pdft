#include "pdft/view/image/kitty/unicode_display.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include "./detail/rowcolumn_diacritics.h"

namespace pdft::view::image::kitty {

void UnicodeDisplay::displayImage(
    media::ImageId id, const IRect& rect, const IRect& image, core::io::OutBuffer& buffer) {
    const auto& [x, y, w, h] = rect;
    const auto& [ix, iy, iw, ih] = image;
    core::log::global()->info("display @ x={},y={},w={},h={}", x, y, w, h);
    core::log::global()->info("display I x={},y={},w={},h={}", ix, iy, iw, ih);

    PDFT_VERIFY(x >= 0 && y >= 0, "rect offset must not be negative: x={},y={}", x, y);
    PDFT_VERIFY(ix >= 0 && iy >= 0, "image offset must not be negative: x={},y={}", x, y);

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},
        {"U", "1"},
        {"i", std::to_string(id)},
        {"r", std::to_string(ih)},
        {"c", std::to_string(iw)},
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

    uint32_t left = x;
    uint32_t top = y;
    uint32_t num_rows = h;
    uint32_t num_cols = w;

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
                detail::RowColDiacritics[iy + row - top],
                detail::RowColDiacritics[ix + col - left],
                IdChar);
        }
    }

    core::io::write(buffer, "\033[39m");
}

}  // namespace pdft::view::image::kitty
