#pragma once

#include "pdft/io/console_reader.h"

#include "pdft/core/module/module.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::io {

class PosixConsoleReader : public ConsoleReader, public Module {
 public:
    struct Config {
        int fd = STDIN_FILENO;
    };

    PosixConsoleReader(asio::io_context& context, Config config);

    using ConsoleReader::read;

    Async<Status> read(size_t num_bytes, std::string& buffer) override;

 private:
    asio::posix::stream_descriptor stdin_;
};

}  // namespace pdft::io
