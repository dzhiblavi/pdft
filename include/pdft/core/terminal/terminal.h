#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/core/terminal/cursor_position.h"

namespace pdft::core::terminal {

struct Dimensions {
    uint32_t rows;
    uint32_t columns;
    uint32_t height_px;
    uint32_t width_px;
    uint32_t cell_height_px;
    uint32_t cell_width_px;
};

class Terminal {
 public:
    virtual ~Terminal() = default;

    virtual void clear(OutBuffer& buffer) = 0;
    virtual void moveCursor(CursorPosition position, OutBuffer& buffer) = 0;
};

Dimensions getTerminalDimensions();

}  // namespace pdft::core::terminal
