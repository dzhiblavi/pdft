#include "pdft/core/io/posix_console.h"
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

TEST_CASE("testPosixConsoleWrite", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = PosixConsole(context, mock->inFd(), mock->outFd());

        co_await console.write("hello, world!");
        CHECK(mock->written() == "hello, world!");
    });
}

TEST_CASE("testPosixConsoleRead", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = PosixConsole(context, mock->inFd(), mock->outFd());

        mock->put("abacaba");
        auto out = co_await console.read(3) | result::unwrap;
        CHECK(out == "aba");
    });
}

TEST_CASE("testPosixConsoleMixed", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        asio::error_code ec;
        std::string read;
        auto mock = test::makeMockConsole();
        auto console = PosixConsole(context, mock->inFd(), mock->outFd());

        mock->put("hello");
        read = co_await console.read(3) | result::unwrap;
        CHECK(read == "hel");

        co_await console.write("test");
        CHECK(mock->written() == "test");

        mock->put("abacaba");
        read = co_await console.read(5) | result::unwrap;
        CHECK(read == "loaba");

        read = co_await console.read(4) | result::unwrap;
        CHECK(read == "caba");
    });
}

}  // namespace pdft::io
