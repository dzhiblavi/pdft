#include "pdft/core/terminal/ansi/terminal.h"

namespace pdft::core::terminal::ansi {

void Terminal::clear(io::OutBuffer& buffer) {
    io::write(buffer, "\x1b[2J\x1b[H");
}

void Terminal::moveCursor(CursorPosition position, io::OutBuffer& buffer) {
    io::write(buffer, "\x1b[", position.rows + 1, ';', position.columns + 1, "H");
}

}  // namespace pdft::core::terminal::ansi
