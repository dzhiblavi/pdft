#include "pdft/tui/kitty/apc/regular_apc_executor.h"

#include "tests/pdft/core/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

namespace pdft::tui {

TEST_CASE("testExecuteNoPayload", "") {
    core::Context ctx;
    core::io::OutBuffer buffer;
    auto executor = RegularAPCExecutor();

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

    CHECK(buffer.view() == "\033_Gm=1,a=p,C=1;\033\\");
}

TEST_CASE("testExecuteWithPayload", "") {
    core::Context ctx;
    core::io::OutBuffer buffer;
    auto executor = RegularAPCExecutor();

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

    CHECK(buffer.view() == "\033_Gm=1,a=p,C=1;hello\033\\");
}

}  // namespace pdft::tui
