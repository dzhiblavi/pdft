#include "pdft/io/posix_console_reader.h"
#include "pdft/core/asio_as_tuple.h"

#include "platform/core/result.h"
#include "platform/module/register.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

PosixConsoleReader::PosixConsoleReader(asio::io_context& context, Config config)
    : stdin_(context, config.fd) {}

platform::Async<platform::Status> PosixConsoleReader::read(size_t num_bytes, std::string& out) {
    out.resize(num_bytes);
    auto [ec, _] = co_await asio::async_read(stdin_, asio::buffer(out), AsioCoroAsTuple);

    if (ec) {
        co_return result::makeError(platform::errorCodeToString(ec));
    }

    co_return result::unit;
}

}  // namespace pdft::io

PLATFORM_REGISTER_MODULE(pdft::io::PosixConsoleReader);
