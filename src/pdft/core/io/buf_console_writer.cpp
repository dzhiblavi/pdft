#include "pdft/core/io/buf_console_writer.h"
#include "pdft/core/result/make/ok.h"

#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

namespace pdft::io {

BufConsoleWriter::BufConsoleWriter(OutBuffer& buffer) : buffer_(buffer) {}

AsyncStatus BufConsoleWriter::write(std::string_view bytes) {
    io::write(buffer_, bytes);
    co_return result::ok();
}

AsyncStatus BufConsoleWriter::writeMany(std::vector<asio::const_buffer> buffers) {
    for (const auto& buf : buffers) {
        std::string_view view((const char*)buf.data(), buf.size());
        io::write(buffer_, view);
    }
    co_return result::ok();
}

}  // namespace pdft::io
