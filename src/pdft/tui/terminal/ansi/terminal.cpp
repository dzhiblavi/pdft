#include "pdft/tui/terminal/ansi/terminal.h"

namespace pdft::tui::terminal::ansi {

void Terminal::clear(core::io::OutBuffer& buffer) {
    core::io::write(buffer, "\x1b[2J\x1b[H");
}

void Terminal::setCursorPosition(CursorPosition position, core::io::OutBuffer& buffer) {
    core::io::write(buffer, "\x1b[", position.rows + 1, ';', position.columns + 1, "H");
}

void Terminal::moveCursorHorizontally(int32_t shift, core::io::OutBuffer& buffer) {
    if (shift == 0) {
        return;
    }

    if (shift > 0) {
        core::io::write(buffer, "\x1b[", +shift, "C");
    } else {
        core::io::write(buffer, "\x1b[", -shift, "D");
    }
}

void Terminal::moveCursorVertically(int32_t shift, core::io::OutBuffer& buffer) {
    if (shift == 0) {
        return;
    }

    if (shift > 0) {
        core::io::write(buffer, "\x1b[", +shift, "A");
    } else {
        core::io::write(buffer, "\x1b[", -shift, "B");
    }
}

}  // namespace pdft::tui::terminal::ansi
