#pragma once

#include <cstdint>

namespace pdft::view {

// A rectangle with integer dimensions
struct IRect {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;

    bool operator==(const IRect&) const = default;
};

IRect offsetRect(const IRect& rect, int32_t x_offset, int32_t y_offset);

}  // namespace pdft::view
