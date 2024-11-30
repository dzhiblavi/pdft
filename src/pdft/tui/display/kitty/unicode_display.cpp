#include "pdft/tui/display/kitty/unicode_display.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include "./detail/rowcolumn_diacritics.h"

namespace pdft::tui::display::kitty {

void UnicodeDisplay::displayImage(
    ImageId id, const IRect& rect, const FRect& image_subarea, Context& ctx) {
    log::debug("display @ {}", rect);
    PDFT_VERIFY(isLoaded(id));

    const auto& [x, y, w, h] = rect;
    PDFT_VERIFY(x >= 0 && y >= 0, "rect offset must not be negative: x={},y={}", x, y);
    PDFT_VERIFY(w >= 0 && h >= 0, "rect dimensions must not be negative: w={},h={}", w, h);

    if (w == 0 || h == 0) {
        return;
    }

    const auto& [rix, riy, riw, rih] = image_subarea;
    log::debug("display I {}", image_subarea);
    PDFT_VERIFY(rix >= 0 && riy >= 0, "image offset must not be negative: x={},y={}", rix, riy);
    PDFT_VERIFY(riw > 0 && rih > 0, "image dimensions must be positive: w={},h={}", riw, rih);

    const uint32_t image_cols = std::roundf(w / riw);
    const uint32_t image_rows = std::roundf(h / rih);
    log::debug("display ISZ wc={},hc={}", image_cols, image_rows);

    const uint32_t offset_cols = image_cols * rix;
    const uint32_t offset_rows = image_rows * riy;
    log::debug("display IOF c={},r={}", offset_cols, offset_rows);

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},                 // put image to the display
        {"U", "1"},                 // use unicode placeholders (create a *virtual* placement)
        {"C", "1"},                 // do not move the cursor
        {"i", std::to_string(id)},  // image id
        {"r", std::to_string(image_rows)},  // rows occupied by the image
        {"c", std::to_string(image_cols)},  // columns occupied by the image
    };

    // Create a virtual PagePlacement
    executor_->execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        ctx.control_buffer);

    // Fill the image space with special unicode characters
    const std::string_view ImagePlaceholder = "\U0010EEEE";
    const std::string_view IdChar = detail::RowColDiacritics[(id >> 24) & 255];

    core::io::write(
        ctx.display_buffer,
        "\033[38:2:",
        (id >> 16) & 255,
        ":",
        (id >> 8) & 255,
        ":",
        (id >> 0) & 255,
        "m");

    for (uint32_t row = 0; row < h; ++row) {
        ctx.terminal.setCursorPosition(
            tui::terminal::CursorPosition{
                .rows = y + row,
                .columns = static_cast<uint32_t>(x),
            },
            ctx.display_buffer);

        for (uint32_t col = 0; col < w; ++col) {
            core::io::write(
                ctx.display_buffer,
                ImagePlaceholder,
                detail::RowColDiacritics[offset_rows + row],
                detail::RowColDiacritics[offset_cols + col],
                IdChar);
        }
    }

    core::io::write(ctx.display_buffer, "\033[39m");
}

}  // namespace pdft::tui::display::kitty
