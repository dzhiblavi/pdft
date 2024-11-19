#pragma once

#include <cstdint>

namespace pdft::display {

struct Position {
    uint32_t rows;
    uint32_t columns;

    // These may be negative and need to be cropped if so.
    int32_t top;
    int32_t left;

    bool operator==(const Position&) const = default;
};

}  // namespace pdft::display
