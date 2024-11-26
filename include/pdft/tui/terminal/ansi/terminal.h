#pragma once

#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui::terminal::ansi {

class Terminal : public terminal::Terminal {
 public:
    void clear(core::io::OutBuffer& buffer) override;
    void setCursorPosition(CursorPosition position, core::io::OutBuffer& buffer) override;
    void moveCursorHorizontally(int32_t shift, core::io::OutBuffer& buffer) override;
    void moveCursorVertically(int32_t shift, core::io::OutBuffer& buffer) override;
};

}  // namespace pdft::tui::terminal::ansi
