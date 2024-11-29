#include "pdft/tui/display/kitty/apc/regular/executor.h"

#include "tests/pdft/core/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

namespace pdft::tui::display::kitty::apc::regular {

TEST_CASE("testExecuteNoPayload", "") {
    core::io::OutBuffer buffer;
    auto executor = Executor();

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

    CHECK(buffer.view() == "\033_Gm=1,a=p,C=1;\033\\");
}

TEST_CASE("testExecuteWithPayload", "") {
    core::io::OutBuffer buffer;
    auto executor = Executor();

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

    CHECK(buffer.view() == "\033_Gm=1,a=p,C=1;hello\033\\");
}

}  // namespace pdft::tui::display::kitty::apc::regular
