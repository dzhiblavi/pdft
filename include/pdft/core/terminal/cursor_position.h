#pragma once

#include <cstdint>

namespace pdft::core::terminal {

struct CursorPosition {
    uint32_t rows;
    uint32_t columns;
};

}  // namespace pdft::core::terminal
