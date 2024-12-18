#include "pdft/io/posix_console_reader.h"

#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep

#include "tests/pdft/io/mock_fd_console.h"
#include "tests/pdft/run_coroutine.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::io {

TEST_CASE("PosixConsoleReader/read", "") {
    tests::runCoroutine([](auto& context) -> asio::awaitable<void> {
        auto mock = test::makeMockConsole();
        auto console = PosixConsoleReader(context, {.fd = mock->inFd()});

        mock->put("abacaba");
        auto out = co_await console.read(3) | result::unwrap;
        CHECK(out == "aba");
    });
}

}  // namespace pdft::io
