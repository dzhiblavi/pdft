#pragma once

#include "pdft/io/console_reader.h"

#include "platform/module/module.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::io {

class PosixConsoleReader : public ConsoleReader, public platform::rt::Module {
 public:
    struct Config {
        int fd = STDIN_FILENO;
    };

    PosixConsoleReader(asio::io_context& context, Config config);

    using ConsoleReader::read;

    platform::Async<platform::Status> read(size_t num_bytes, std::string& buffer) override;

 private:
    asio::posix::stream_descriptor stdin_;
};

}  // namespace pdft::io
