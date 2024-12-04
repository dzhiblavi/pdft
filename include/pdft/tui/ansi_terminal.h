#pragma once

#include "pdft/tui/terminal.h"

namespace pdft::tui {

class AnsiTerminal : public Terminal {
 public:
    void clear(core::io::OutBuffer& buffer) const override;
    void setCursorPosition(CursorPosition position, core::io::OutBuffer& buffer) const override;
    void moveCursorHorizontally(int32_t shift, core::io::OutBuffer& buffer) const override;
    void moveCursorVertically(int32_t shift, core::io::OutBuffer& buffer) const override;
    void hideCursor(core::io::OutBuffer& buffer) const override;
    void showCursor(core::io::OutBuffer& buffer) const override;
};

}  // namespace pdft::tui
