#pragma once

#include "pdft/core/io/console_writer.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::io {

class PosixConsoleWriter : public ConsoleWriter {
 public:
    PosixConsoleWriter(asio::io_context& context, int out_fd);

    using ConsoleWriter::write;

    AsyncStatus write(std::string_view bytes) override;
    AsyncStatus writeMany(std::vector<asio::const_buffer> bytes) override;

 private:
    asio::posix::stream_descriptor stdout_;
};

}  // namespace pdft::io
