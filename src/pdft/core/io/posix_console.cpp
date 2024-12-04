#include "pdft/core/io/posix_console.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

PosixConsole::PosixConsole(asio::io_context& context, int stdin_fd, int stdout_fd)
    : stdin_(context, stdin_fd)
    , stdout_(context, stdout_fd) {}

AsyncStatus PosixConsole::write(std::string_view bytes) {
    auto [ec, _] = co_await asio::async_write(stdout_, asio::buffer(bytes), asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok();
}

AsyncStatus PosixConsole::writeMany(std::vector<asio::const_buffer> buffers) {
    auto [ec, _] = co_await asio::async_write(stdout_, std::move(buffers), asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok();
}

AsyncStatus PosixConsole::read(size_t num_bytes, std::string& out) {
    out.resize(num_bytes);
    auto [ec, _] = co_await asio::async_read(stdin_, asio::buffer(out), asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok();
}

}  // namespace pdft::io
