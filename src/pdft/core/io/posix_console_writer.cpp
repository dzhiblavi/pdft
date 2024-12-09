#include "pdft/core/io/posix_console_writer.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

PosixConsoleWriter::PosixConsoleWriter(asio::io_context& context, int out_fd)
    : stdout_(context, out_fd) {}

AsyncStatus PosixConsoleWriter::write(std::string_view bytes) {
    auto [ec, _] = co_await asio::async_write(stdout_, asio::buffer(bytes), asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok();
}

AsyncStatus PosixConsoleWriter::writeMany(std::vector<asio::const_buffer> buffers) {
    auto [ec, _] = co_await asio::async_write(stdout_, std::move(buffers), asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok();
}

}  // namespace pdft::io
