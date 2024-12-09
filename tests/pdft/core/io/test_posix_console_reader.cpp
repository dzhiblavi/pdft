#include "pdft/core/io/posix_console_reader.h"
#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"
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

TEST_CASE("testPosixConsoleRead", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = PosixConsoleReader(context, mock->inFd());

        mock->put("abacaba");
        auto out = co_await console.read(3) | result::unwrap;
        CHECK(out == "aba");
    });
}

}  // namespace pdft::io
