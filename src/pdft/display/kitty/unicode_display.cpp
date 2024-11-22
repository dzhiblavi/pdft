#include "pdft/display/kitty/unicode_display.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include "./detail/rowcolumn_diacritics.h"

namespace pdft::display::kitty {

asio::awaitable<void> UnicodeDisplay::displayImage(
    media::ImageId id,
    const view::IRect& rect,
    const view::FRect& image_subarea,
    core::io::OutBuffer& buffer) {
    PDFT_VERIFY(isLoaded(id));

    const auto& [x, y, w, h] = rect;
    core::log::global()->info("display @ x={},y={},w={},h={}", x, y, w, h);
    PDFT_VERIFY(x >= 0 && y >= 0, "rect offset must not be negative: x={},y={}", x, y);
    PDFT_VERIFY(w >= 0 && h >= 0, "rect dimensions must not be negative: w={},h={}", w, h);

    if (w == 0 || h == 0) {
        co_return;
    }

    const auto& [rix, riy, riw, rih] = image_subarea;
    core::log::global()->info("display I rx={},ry={},rw={},rh={}", rix, riy, riw, rih);
    PDFT_VERIFY(rix >= 0 && riy >= 0, "image offset must not be negative: x={},y={}", rix, riy);
    PDFT_VERIFY(riw > 0 && rih > 0, "image dimensions must be positive: w={},h={}", riw, rih);

    uint32_t image_cols = std::roundf(w / riw);
    uint32_t image_rows = std::roundf(h / rih);
    core::log::global()->info("display ISZ wc={},hc={}", image_cols, image_rows);

    uint32_t offset_cols = image_cols * rix;
    uint32_t offset_rows = image_rows * riy;
    core::log::global()->info("display IOF c={},r={}", offset_cols, offset_rows);

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},
        {"U", "1"},
        {"i", std::to_string(id)},
        {"r", std::to_string(image_rows)},
        {"c", std::to_string(image_cols)},
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

    // Fill the image space with special unicode characters
    const std::string_view ImagePlaceholder = "\U0010EEEE";
    const std::string_view IdChar = detail::RowColDiacritics[(id >> 24) & 255];

    core::io::write(
        buffer, "\033[38:2:", (id >> 16) & 255, ":", (id >> 8) & 255, ":", (id >> 0) & 255, "m");

    for (uint32_t row = 0; row < h; ++row) {
        terminal_->moveCursor(
            core::terminal::CursorPosition{
                .rows = y + row,
                .columns = static_cast<uint32_t>(x),
            },
            buffer);

        for (uint32_t col = 0; col < w; ++col) {
            core::io::write(
                buffer,
                ImagePlaceholder,
                detail::RowColDiacritics[offset_rows + row],
                detail::RowColDiacritics[offset_cols + col],
                IdChar);
        }
    }

    core::io::write(buffer, "\033[39m");
}

}  // namespace pdft::display::kitty
