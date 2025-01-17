#include "pdft/tui/kitty/unicode_kitty_display.h"

#include "platform/core/assert.h"
#include "platform/log.h"
#include "platform/module/register.h"

#include "./detail/rowcolumn_diacritics.h"

namespace pdft::tui {

using namespace platform;

void UnicodeKittyDisplay::displayImage(
    platform::trace::Context& ctx,
    ImageId id,
    const IRect& rect,
    const FRect& image_subarea,
    BufPair& buffers) {
    platform::verify(isLoaded(id));

    const auto& [x, y, w, h] = rect;
    platform::verify(x >= 0 && y >= 0, "rect offset must not be negative: x={},y={}", x, y);
    platform::verify(w >= 0 && h >= 0, "rect dimensions must not be negative: w={},h={}", w, h);

    if (w == 0 || h == 0) {
        return;
    }

    const auto& [rix, riy, riw, rih] = image_subarea;
    log::debug(this, "@={} SA={}", rect, image_subarea);
    platform::verify(
        rix >= 0 && riy >= 0, "image offset must not be negative: x={},y={}", rix, riy);
    platform::verify(riw > 0 && rih > 0, "image dimensions must be positive: w={},h={}", riw, rih);

    const uint32_t image_cols = std::roundf(w / riw);  // NOLINT
    const uint32_t image_rows = std::roundf(h / rih);  // NOLINT
    const uint32_t offset_cols = image_cols * rix;     // NOLINT
    const uint32_t offset_rows = image_rows * riy;     // NOLINT

    log::debug(this, "oc={},or={},wc={},hr={}", offset_cols, offset_rows, image_cols, image_rows);

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
        ctx,
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffers.control_buffer);

    // Fill the image space with special unicode characters
    const std::string_view image_placeholder = "\U0010EEEE";
    const std::string_view id_char = detail::RowColDiacritics[(id >> 24) & 255];

    auto _ = ctx.push("unicode_placement");

    io::write(
        buffers.display_buffer,
        "\033[38:2:",
        (id >> 16) & 255,  // NOLINT
        ":",
        (id >> 8) & 255,  // NOLINT
        ":",
        (id >> 0) & 255,  // NOLINT
        "m");

    for (uint32_t row = 0; row < h; ++row) {
        terminal_->setCursorPosition(
            tui::CursorPosition{
                .rows = y + row,
                .columns = static_cast<uint32_t>(x),
            },
            buffers.display_buffer);

        for (uint32_t col = 0; col < w; ++col) {
            io::write(
                buffers.display_buffer,
                image_placeholder,
                detail::RowColDiacritics[offset_rows + row],
                detail::RowColDiacritics[offset_cols + col],
                id_char);
        }
    }

    io::write(buffers.display_buffer, "\033[39m");
}

}  // namespace pdft::tui

PLATFORM_REGISTER_MODULE(pdft::tui::UnicodeKittyDisplay);
