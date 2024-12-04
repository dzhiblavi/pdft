#pragma once

#include "pdft/core/io/console.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::io {

class PosixConsole : public Console {
 public:
    explicit PosixConsole(asio::io_context& context, int stdin_fd, int stdout_fd);

    using Console::read;
    using Console::write;

    AsyncStatus write(std::string_view bytes) override;
    AsyncStatus writeMany(std::vector<asio::const_buffer> bytes) override;
    AsyncStatus read(size_t num_bytes, std::string& buffer) override;

 private:
    asio::posix::stream_descriptor stdin_;
    asio::posix::stream_descriptor stdout_;
};

}  // namespace pdft::io
