#include "pdft/core/apc/tmux/apc_executor.h"
#include "tests/pdft/core/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

namespace pdft::core::apc::tmux {

TEST_CASE("testExecuteNoPayload", "") {
    io::OutBuffer buffer;
    auto executor = APCExecutor({}, nullptr);

    std::pair<std::string_view, std::string> control_data[] = {
        {"m", "1"},
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "\033Ptmux;\033\033_Gm=1,a=p,C=1;\a\033\\");
}

TEST_CASE("testExecuteWithPayload", "") {
    io::OutBuffer buffer;
    auto executor = APCExecutor({}, nullptr);

    std::pair<std::string_view, std::string> control_data[] = {
        {"m", "1"},
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .payload = "hello",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "\033Ptmux;\033\033_Gm=1,a=p,C=1;hello\a\033\\");
}

}  // namespace pdft::core::apc::tmux
