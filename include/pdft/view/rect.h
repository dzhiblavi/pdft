#pragma once

#include <concepts>
#include <cstdint>

namespace pdft::view {

template <typename T>
requires std::integral<T> || std::floating_point<T>
struct Rect {
    T x;
    T y;
    T width;
    T height;

    bool operator==(const Rect&) const = default;
};

// A rectangle with non-negative integer dimensions
using IRect = Rect<uint32_t>;

// A rectangle with float dimensions
using FRect = Rect<float>;

template <typename T>
requires std::integral<T> || std::floating_point<T>
Rect<T> offsetRect(const Rect<T>& rect, T x_offset, T y_offset) {
    const auto& [x, y, w, h] = rect;

    return IRect{
        .x = x + x_offset,
        .y = y + y_offset,
        .width = w,
        .height = h,
    };
}

}  // namespace pdft::view
