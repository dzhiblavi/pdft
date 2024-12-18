#include "pdft/io/posix_console_writer.h"

#include "tests/pdft/io/mock_fd_console.h"
#include "tests/pdft/run_coroutine.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::io {

TEST_CASE("PosixConsoleWriter/write", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = PosixConsoleWriter(context, {.fd = mock->outFd()});

        co_await console.write("hello, world!");
        CHECK(mock->written() == "hello, world!");
    });
}

}  // namespace pdft::io
