#include "pdft/tui/kitty/native_display.h"

#include "pdft/core/assert.h"
#include "pdft/core/log.h"

namespace pdft::tui {

void NativeKittyDisplay::displayImage(
    Context& ctx, ImageId id, const IRect& rect, const FRect& image_subarea, BufPair& buffers) {
    PDFT_VERIFY(isLoaded(id));

    const auto& [x, y, w, h] = rect;
    PDFT_VERIFY(x >= 0 && y >= 0, "rect offset must not be negative: x={},y={}", x, y);
    PDFT_VERIFY(w >= 0 && h >= 0, "rect dimensions must not be negative: w={},h={}", w, h);

    if (w == 0 || h == 0) {
        return;
    }

    const auto& [rix, riy, riw, rih] = image_subarea;
    log::debug(ctx, "@={} SA={}", rect, image_subarea);
    PDFT_VERIFY(rix >= 0 && riy >= 0, "image offset must not be negative: x={},y={}", rix, riy);
    PDFT_VERIFY(riw > 0 && rih > 0, "image dimensions must be positive: w={},h={}", riw, rih);

    const auto& image_size = loaded_[id];

    const FRect ipx = {
        .x = std::floor(image_size.width_px * riw * rix),
        .y = std::floor(image_size.height_px * rih * riy),
        .width = std::floor(image_size.width_px * riw),
        .height = std::floor(image_size.height_px * rih),
    };

    log::debug(ctx, "wpx={},hpx={},SA={}", image_size.width_px, image_size.height_px, ipx);

    // x, y, w, h (pixels) control the subrectangle of the image
    // c, r (columns and rows) control the rectangle at which the image will be displayed
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},                         // put image to the display
        {"C", "1"},                         // do not move the cursor
        {"i", std::to_string(id)},          // image id
        {"c", std::to_string(w - 1)},       // columns occupied by the image
        {"r", std::to_string(h - 1)},       // rows occupied by the image
        {"y", std::to_string(ipx.y)},       // vertical offset inside the image in pixels
        {"x", std::to_string(ipx.x)},       // horizontal offset inside the image in pixels
        {"w", std::to_string(ipx.width)},   // height of the image subarea in pixels
        {"h", std::to_string(ipx.height)},  // width of the image subarea in pixels
    };

    // Position cursor to the top left corner
    terminal_->setCursorPosition(
        tui::CursorPosition{
            .rows = static_cast<uint32_t>(y),
            .columns = static_cast<uint32_t>(x),
        },
        buffers.display_buffer);

    // Create a placement
    executor_->execute(
        ctx,
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffers.display_buffer);
}

}  // namespace pdft::tui
