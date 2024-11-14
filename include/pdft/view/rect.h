#pragma once

#include <cstdint>

namespace pdft::view {

// A rectangle with integer dimensions
struct IRect {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;

    bool operator==(const IRect&) const = default;
};

}  // namespace pdft::view
