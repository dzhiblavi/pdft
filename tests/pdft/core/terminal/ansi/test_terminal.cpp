#include "pdft/core/terminal/ansi/terminal.h"
#include "tests/pdft/core/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

namespace pdft::core::terminal::ansi {

TEST_CASE("testMoveCursor", "") {
    io::OutBuffer buffer;
    auto terminal = Terminal();

    terminal.moveCursor(CursorPosition{10, 20}, buffer);
    CHECK(buffer.view() == "\x1b[10;20H");
}

TEST_CASE("testClearScreen", "") {
    io::OutBuffer buffer;
    auto terminal = Terminal();

    terminal.clear(buffer);
    CHECK(buffer.view() == "\x1b[2J\x1b[H");
}

}  // namespace pdft::core::terminal::ansi
