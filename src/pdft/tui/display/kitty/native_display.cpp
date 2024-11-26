#include "pdft/tui/display/kitty/native_display.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

namespace pdft::tui::display::kitty {

void NativeDisplay::displayImage(
    image::ImageId id,
    const IRect& rect,
    const FRect& image_subarea,
    Context& ctx) {
    PDFT_VERIFY(isLoaded(id));

    const auto& [x, y, w, h] = rect;
    log::debug("display @ x={},y={},w={},h={}", x, y, w, h);
    PDFT_VERIFY(x >= 0 && y >= 0, "rect offset must not be negative: x={},y={}", x, y);
    PDFT_VERIFY(w >= 0 && h >= 0, "rect dimensions must not be negative: w={},h={}", w, h);

    if (w == 0 || h == 0) {
        return;
    }

    const auto& [rix, riy, riw, rih] = image_subarea;
    log::debug("display I rx={},ry={},rw={},rh={}", rix, riy, riw, rih);
    PDFT_VERIFY(rix >= 0 && riy >= 0, "image offset must not be negative: x={},y={}", rix, riy);
    PDFT_VERIFY(riw > 0 && rih > 0, "image dimensions must be positive: w={},h={}", riw, rih);

    const auto& image_size = loaded_[id];
    log::debug("display IS wpx={},hpx={}", image_size.width_px, image_size.height_px);

    uint32_t iw_px = std::floor(image_size.width_px * riw);
    uint32_t ih_px = std::floor(image_size.height_px * rih);
    uint32_t ix_px = std::floor(iw_px * rix);
    uint32_t iy_px = std::floor(ih_px * riy);
    log::debug("display Ipx xpx={},ypx={},wpx={},hpx={}", ix_px, iy_px, iw_px, ih_px);

    // x, y, w, h (pixels) control the subrectangle of the image
    // c, r (columns and rows) control the rectangle at which the image will be displayed
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},                    // put image to the display
        {"C", "1"},                    // do not move the cursor
        {"i", std::to_string(id)},     // image id
        {"c", std::to_string(w - 1)},  // columns occupied by the image
        {"r", std::to_string(h - 1)},  // rows occupied by the image
        {"y", std::to_string(iy_px)},  // vertical offset inside the image in pixels
        {"x", std::to_string(ix_px)},  // horizontal offset inside the image in pixels
        {"w", std::to_string(iw_px)},  // height of the image subarea in pixels
        {"h", std::to_string(ih_px)},  // width of the image subarea in pixels
    };

    // Position cursor to the top left corner
    ctx.terminal.setCursorPosition(
        tui::terminal::CursorPosition{
            .rows = static_cast<uint32_t>(y),
            .columns = static_cast<uint32_t>(x),
        },
        ctx.buffer);

    // Create a placement
    executor_->execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        ctx.buffer);
}

}  // namespace pdft::tui::display::kitty
