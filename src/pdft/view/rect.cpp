#include "pdft/view/rect.h"

namespace pdft::view {

IRect offsetRect(const IRect& rect, int32_t x_offset, int32_t y_offset) {
    const auto& [x, y, w, h] = rect;

    return IRect{
        .x = x + x_offset,
        .y = y + y_offset,
        .width = w,
        .height = h,
    };
}

}  // namespace pdft::view
