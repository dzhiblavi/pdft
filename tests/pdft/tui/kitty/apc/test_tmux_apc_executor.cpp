#include "pdft/tui/kitty/apc/tmux_apc_executor.h"

#include "tests/pdft/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

namespace pdft::tui {

TEST_CASE("TmuxAPCExecutor/control/no-payload", "") {
    Context ctx;
    io::OutBuffer buffer;
    auto executor = TmuxAPCExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"m", "1"},
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        ctx,
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "\033Ptmux;\033\033_Gm=1,a=p,C=1;\a\033\\");
}

TEST_CASE("TmuxAPCExecutor/control/payload") {
    Context ctx;
    io::OutBuffer buffer;
    auto executor = TmuxAPCExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"m", "1"},
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        ctx,
        {
            .command = "_G",
            .payload = "hello",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "\033Ptmux;\033\033_Gm=1,a=p,C=1;hello\a\033\\");
}

}  // namespace pdft::tui
