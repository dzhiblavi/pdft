#pragma once

#include "pdft/tui/terminal.h"

namespace pdft::tui {

class AnsiTerminal : public Terminal {
 public:
    void clear(io::OutBuffer& buffer) const override;
    void setCursorPosition(CursorPosition position, io::OutBuffer& buffer) const override;
    void moveCursorHorizontally(int32_t shift, io::OutBuffer& buffer) const override;
    void moveCursorVertically(int32_t shift, io::OutBuffer& buffer) const override;
    void hideCursor(io::OutBuffer& buffer) const override;
    void showCursor(io::OutBuffer& buffer) const override;
};

}  // namespace pdft::tui
