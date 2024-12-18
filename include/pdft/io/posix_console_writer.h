#pragma once

#include "pdft/io/console_writer.h"

#include "pdft/core/module/module.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::io {

class PosixConsoleWriter : public ConsoleWriter, public Module {
 public:
    struct Config {
        int fd = STDOUT_FILENO;
    };

    PosixConsoleWriter(asio::io_context& context, Config config);

    using ConsoleWriter::write;

    Async<Status> write(std::string_view bytes) override;
    Async<Status> writeMany(std::vector<asio::const_buffer> bytes) override;

 private:
    asio::posix::stream_descriptor stdout_;
};

}  // namespace pdft::io
