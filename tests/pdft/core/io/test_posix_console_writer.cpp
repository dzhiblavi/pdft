#include "pdft/core/io/posix_console_writer.h"
#include "tests/pdft/core/io/mock_fd_console.h"
#include "tests/pdft/run_coroutine.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

#include <asio/experimental/awaitable_operators.hpp>
#include <asio/experimental/channel.hpp>
#include <asio/redirect_error.hpp>

#include <catch2/catch_test_macros.hpp>

namespace pdft::io {

using namespace asio::experimental::awaitable_operators;

TEST_CASE("testPosixConsoleWrite", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = PosixConsoleWriter(context, {.fd = mock->outFd()});

        co_await console.write("hello, world!");
        CHECK(mock->written() == "hello, world!");
    });
}

}  // namespace pdft::io
