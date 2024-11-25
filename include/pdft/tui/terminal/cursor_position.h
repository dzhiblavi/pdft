#pragma once

#include <cstdint>

namespace pdft::tui::terminal {

struct CursorPosition {
    uint32_t rows;
    uint32_t columns;
};

}  // namespace pdft::tui::terminal
