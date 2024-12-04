#include "pdft/core/io/posix_console.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

PosixConsole::PosixConsole(asio::io_context& context, int stdin_fd, int stdout_fd)
    : stdin_(context, stdin_fd)
    , stdout_(context, stdout_fd) {}

AsyncResult<void> PosixConsole::write(std::string_view bytes) {
    co_await asio::async_write(stdout_, asio::buffer(bytes), asio_coro);
    co_return asio::error_code();
}

AsyncResult<void> PosixConsole::writeMany(std::vector<asio::const_buffer> buffers) {
    co_await asio::async_write(stdout_, std::move(buffers), asio_coro);
    co_return asio::error_code();
}

AsyncResult<void> PosixConsole::read(size_t num_bytes, std::string& out) {
    out.resize(num_bytes);
    co_await asio::async_read(stdin_, asio::buffer(out), asio_coro);
    co_return asio::error_code();
}

}  // namespace pdft::io
