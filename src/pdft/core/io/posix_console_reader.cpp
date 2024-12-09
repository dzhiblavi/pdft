#include "pdft/core/io/posix_console_reader.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

PosixConsoleReader::PosixConsoleReader(asio::io_context& context, int in_fd)
    : stdin_(context, in_fd) {}

AsyncStatus PosixConsoleReader::read(size_t num_bytes, std::string& out) {
    out.resize(num_bytes);
    auto [ec, _] = co_await asio::async_read(stdin_, asio::buffer(out), asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok();
}

}  // namespace pdft::io
