#include "pdft/tui/ansi_terminal.h"

namespace pdft::tui {

void AnsiTerminal::clear(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\x1b[2J\x1b[H");
}

void AnsiTerminal::setCursorPosition(CursorPosition position, core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\x1b[", position.rows + 1, ';', position.columns + 1, "H");
}

void AnsiTerminal::moveCursorHorizontally(int32_t shift, core::io::OutBuffer& buffer) const {
    if (shift == 0) {
        return;
    }

    if (shift > 0) {
        core::io::write(buffer, "\x1b[", +shift, "C");
    } else {
        core::io::write(buffer, "\x1b[", -shift, "D");
    }
}

void AnsiTerminal::moveCursorVertically(int32_t shift, core::io::OutBuffer& buffer) const {
    if (shift == 0) {
        return;
    }

    if (shift > 0) {
        core::io::write(buffer, "\x1b[", +shift, "A");
    } else {
        core::io::write(buffer, "\x1b[", -shift, "B");
    }
}

void AnsiTerminal::hideCursor(core::io::OutBuffer& buffer) const {
    // Lehidden
    core::io::write(buffer, "\x1b[?25l");
}

void AnsiTerminal::showCursor(core::io::OutBuffer& buffer) const {
    // HonestlyTheCursorWillBeVisible
    core::io::write(buffer, "\x1b[?25h");
}

}  // namespace pdft::tui
