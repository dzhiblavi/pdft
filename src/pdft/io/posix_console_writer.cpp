#include "pdft/io/posix_console_writer.h"
#include "pdft/core/asio_as_tuple.h"

#include "platform/core/result.h"
#include "platform/module/register.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

PosixConsoleWriter::PosixConsoleWriter(asio::io_context& context, Config config)
    : stdout_(context, config.fd) {}

platform::Async<platform::Status> PosixConsoleWriter::write(std::string_view bytes) {
    auto [ec, _] = co_await asio::async_write(stdout_, asio::buffer(bytes), AsioCoroAsTuple);

    if (ec) {
        co_return result::makeError(platform::errorCodeToString(ec));
    }

    co_return result::unit;
}

platform::Async<platform::Status> PosixConsoleWriter::writeMany(
    std::vector<asio::const_buffer> buffers) {
    auto [ec, _] = co_await asio::async_write(stdout_, std::move(buffers), AsioCoroAsTuple);

    if (ec) {
        co_return result::makeError(platform::errorCodeToString(ec));
    }

    co_return result::unit;
}

}  // namespace pdft::io

PLATFORM_REGISTER_MODULE(pdft::io::PosixConsoleWriter);
