#include "pdft/display/kitty/native_display.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

namespace pdft::display::kitty {

using namespace core::log::exported;

asio::awaitable<void> NativeDisplay::displayImage(
    media::ImageId id,
    const view::IRect& rect,
    const view::FRect& image_subarea,
    core::io::OutBuffer& buffer) {
    PDFT_VERIFY(isLoaded(id));

    const auto& [x, y, w, h] = rect;
    log::info("display @ x={},y={},w={},h={}", x, y, w, h);
    PDFT_VERIFY(x >= 0 && y >= 0, "rect offset must not be negative: x={},y={}", x, y);
    PDFT_VERIFY(w >= 0 && h >= 0, "rect dimensions must not be negative: w={},h={}", w, h);

    if (w == 0 || h == 0) {
        co_return;
    }

    const auto& [rix, riy, riw, rih] = image_subarea;
    log::info("display I rx={},ry={},rw={},rh={}", rix, riy, riw, rih);
    PDFT_VERIFY(rix >= 0 && riy >= 0, "image offset must not be negative: x={},y={}", rix, riy);
    PDFT_VERIFY(riw > 0 && rih > 0, "image dimensions must be positive: w={},h={}", riw, rih);

    const auto& image_size = loaded_[id];
    log::info("display IS wpx={},hpx={}", image_size.width_px, image_size.height_px);

    uint32_t iw_px = std::floor(image_size.width_px * riw);
    uint32_t ih_px = std::floor(image_size.height_px * rih);
    uint32_t ix_px = std::floor(iw_px * rix);
    uint32_t iy_px = std::floor(ih_px * riy);
    log::info("display Ipx xpx={},ypx={},wpx={},hpx={}", ix_px, iy_px, iw_px, ih_px);

    // x, y, w, h (pixels) control the subrectangle of the image
    // c, r (columns and rows) control the rectangle at which the image will be displayed
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},
        {"C", "1"},
        {"i", std::to_string(id)},
        {"c", std::to_string(w - 1)},
        {"r", std::to_string(h - 1)},
        {"y", std::to_string(iy_px)},
        {"x", std::to_string(ix_px)},
        {"w", std::to_string(iw_px)},
        {"h", std::to_string(ih_px)},
    };

    // Position cursor to the top left corner
    terminal_->moveCursor(
        core::terminal::CursorPosition{
            .rows = static_cast<uint32_t>(y),
            .columns = static_cast<uint32_t>(x),
        },
        buffer);

    // Create a placement
    executor_->execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

}  // namespace pdft::display::kitty
