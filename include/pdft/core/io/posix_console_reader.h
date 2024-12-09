#pragma once

#include "pdft/core/io/console_reader.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::io {

class PosixConsoleReader : public ConsoleReader {
 public:
    PosixConsoleReader(asio::io_context& context, int in_fd);

    using ConsoleReader::read;

    AsyncStatus read(size_t num_bytes, std::string& buffer) override;

 private:
    asio::posix::stream_descriptor stdin_;
};

}  // namespace pdft::io
