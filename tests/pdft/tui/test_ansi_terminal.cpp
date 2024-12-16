#include "tests/pdft/core/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

#include "pdft/tui/impl/ansi_terminal.h"

namespace pdft::tui {

TEST_CASE("AnsiTerminal/setCursorPosition") {
    io::OutBuffer buffer;
    auto terminal = AnsiTerminal();

    terminal.setCursorPosition(CursorPosition{1, 2}, buffer);
    CHECK(buffer.view() == "\x1b[2;3H");
}

TEST_CASE("AnsiTerminal/clear") {
    io::OutBuffer buffer;
    auto terminal = AnsiTerminal();

    terminal.clear(buffer);
    CHECK(buffer.view() == "\x1b[2J\x1b[H");
}

}  // namespace pdft::tui
