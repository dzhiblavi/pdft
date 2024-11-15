#include "pdft/core/terminal/ansi/terminal.h"

namespace pdft::core::terminal::ansi {

void Terminal::clear(OutBuffer& buffer) {
    write(buffer, "\x1b[2J\x1b[H");
}

void Terminal::moveCursor(CursorPosition position, OutBuffer& buffer) {
    write(buffer, "\x1b[", position.rows, ';', position.columns, "H");
}

}  // namespace pdft::core::terminal::ansi
