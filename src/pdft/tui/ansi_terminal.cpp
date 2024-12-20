#include "pdft/tui/impl/ansi_terminal.h"

#include "pdft/runtime/module/register.h"

namespace pdft::tui {

void AnsiTerminal::clear(io::OutBuffer& buffer) const {
    io::write(buffer, "\x1b[2J\x1b[H");
}

void AnsiTerminal::setCursorPosition(CursorPosition position, io::OutBuffer& buffer) const {
    io::write(buffer, "\x1b[", position.rows + 1, ';', position.columns + 1, "H");
}

void AnsiTerminal::moveCursorHorizontally(int32_t shift, io::OutBuffer& buffer) const {
    if (shift == 0) {
        return;
    }

    if (shift > 0) {
        io::write(buffer, "\x1b[", +shift, "C");
    } else {
        io::write(buffer, "\x1b[", -shift, "D");
    }
}

void AnsiTerminal::moveCursorVertically(int32_t shift, io::OutBuffer& buffer) const {
    if (shift == 0) {
        return;
    }

    if (shift > 0) {
        io::write(buffer, "\x1b[", +shift, "A");
    } else {
        io::write(buffer, "\x1b[", -shift, "B");
    }
}

void AnsiTerminal::hideCursor(io::OutBuffer& buffer) const {
    // Lehidden
    io::write(buffer, "\x1b[?25l");
}

void AnsiTerminal::showCursor(io::OutBuffer& buffer) const {
    // HonestlyTheCursorWillBeVisible
    io::write(buffer, "\x1b[?25h");
}

}  // namespace pdft::tui

PDFT_REGISTER_MODULE(pdft::tui::AnsiTerminal);
