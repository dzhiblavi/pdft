#include "tests/pdft/core/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

#include "pdft/tui/terminal/ansi/terminal.h"

namespace pdft::tui::terminal::ansi {

TEST_CASE("testMoveCursor", "") {
    core::io::OutBuffer buffer;
    auto terminal = Terminal();

    terminal.moveCursor(CursorPosition{10, 20}, buffer);
    CHECK(buffer.view() == "\x1b[11;21H");
}

TEST_CASE("testClearScreen", "") {
    core::io::OutBuffer buffer;
    auto terminal = Terminal();

    terminal.clear(buffer);
    CHECK(buffer.view() == "\x1b[2J\x1b[H");
}

}  // namespace pdft::tui::terminal::ansi
