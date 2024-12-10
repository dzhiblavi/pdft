#include "pdft/core/io/posix_console_reader.h"
#include "pdft/core/module/register.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

PosixConsoleReader::PosixConsoleReader(asio::io_context& context, Config config)
    : stdin_(context, config.fd) {}

AsyncStatus PosixConsoleReader::read(size_t num_bytes, std::string& out) {
    out.resize(num_bytes);
    auto [ec, _] = co_await asio::async_read(stdin_, asio::buffer(out), asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok();
}

}  // namespace pdft::io

PDFT_REGISTER_MODULE(pdft::io::PosixConsoleReader);
