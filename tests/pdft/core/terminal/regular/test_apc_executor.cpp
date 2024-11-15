#include "pdft/core/terminal/regular/apc_executor.h"
#include "tests/pdft/core/io/mock_console.h"
#include "tests/pdft/run_coroutine.h"

namespace pdft::core::terminal::regular {

TEST_CASE("testExecuteNoPayload", "") {
    OutBuffer buffer;
    auto executor = APCExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"m", "1"},
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .payload_b64 = "",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "\033_Gm=1,a=p,C=1;\033\\");
}

TEST_CASE("testExecuteWithPayload", "") {
    OutBuffer buffer;
    auto executor = APCExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"m", "1"},
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .payload_b64 = "hello",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "\033_Gm=1,a=p,C=1;hello\033\\");
}

}  // namespace pdft::core::terminal::regular
