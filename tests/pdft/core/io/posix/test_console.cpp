#include "pdft/core/io/posix/console.h"
#include "tests/pdft/core/io/posix/mock_console.h"
#include "tests/pdft/run_coroutine.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

#include <asio/experimental/awaitable_operators.hpp>
#include <asio/experimental/channel.hpp>
#include <asio/redirect_error.hpp>

#include <catch2/catch_test_macros.hpp>

namespace pdft::core::io {

using namespace asio::experimental::awaitable_operators;

TEST_CASE("testPosixConsoleWrite", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = posix::Console(context, mock->inFd(), mock->outFd());

        co_await console.write("hello, world!");
        CHECK(mock->written() == "hello, world!");
    });
}

TEST_CASE("testPosixConsoleRead", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = posix::Console(context, mock->inFd(), mock->outFd());

        mock->put("abacaba");
        auto out = co_await console.read(3);
        CHECK(std::get<1>(out) == "aba");
    });
}

TEST_CASE("testPosixConsoleMixed", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        asio::error_code ec;
        std::string read;
        auto mock = test::makeMockConsole();
        auto console = posix::Console(context, mock->inFd(), mock->outFd());

        mock->put("hello");
        std::tie(ec, read) = co_await console.read(3);
        CHECK(read == "hel");

        co_await console.write("test");
        CHECK(mock->written() == "test");

        mock->put("abacaba");
        std::tie(ec, read) = co_await console.read(5);
        CHECK(read == "loaba");

        std::tie(ec, read) = co_await console.read(4);
        CHECK(read == "caba");
    });
}

}  // namespace pdft::core::io
