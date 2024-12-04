#pragma once

#include "pdft/tui/cursor_position.h"
#include "pdft/tui/rect.h"

#include "pdft/core/io/buffer.h"

namespace pdft::tui {

struct TerminalDimensions {
    uint32_t rows;
    uint32_t columns;
    uint32_t height_px;
    uint32_t width_px;
    uint32_t cell_height_px;
    uint32_t cell_width_px;

    IRect rect() const {
        return {
            .width = columns,
            .height = rows,
        };
    }
};

class Terminal {
 public:
    virtual ~Terminal() = default;

    TerminalDimensions getDimensions() const;

    virtual void clear(core::io::OutBuffer& buffer) const = 0;
    virtual void setCursorPosition(CursorPosition position, core::io::OutBuffer& buffer) const = 0;
    virtual void moveCursorHorizontally(int32_t shift, core::io::OutBuffer& buffer) const = 0;
    virtual void moveCursorVertically(int32_t shift, core::io::OutBuffer& buffer) const = 0;
    virtual void hideCursor(core::io::OutBuffer& buffer) const = 0;
    virtual void showCursor(core::io::OutBuffer& buffer) const = 0;
};

}  // namespace pdft::tui
