#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/tui/layout/rect.h"
#include "pdft/tui/style/style.h"
#include "pdft/tui/terminal/cursor_position.h"

namespace pdft::tui::terminal {

struct Dimensions {
    uint32_t rows;
    uint32_t columns;
    uint32_t height_px;
    uint32_t width_px;
    uint32_t cell_height_px;
    uint32_t cell_width_px;

    layout::IRect rect() const {
        return {
            .width = columns,
            .height = rows,
        };
    }
};

class Terminal {
 public:
    virtual ~Terminal() = default;

    Dimensions getDimensions() const;

    virtual void clear(core::io::OutBuffer& buffer) = 0;
    virtual void moveCursor(CursorPosition position, core::io::OutBuffer& buffer) = 0;
    virtual void patchStyle(style::Patch patch, core::io::OutBuffer& buffer) = 0;
};

}  // namespace pdft::tui::terminal
