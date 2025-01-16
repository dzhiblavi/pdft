#pragma once

#include "pdft/io/console_writer.h"

#include "platform/module/module.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::io {

class PosixConsoleWriter : public ConsoleWriter, public platform::rt::Module {
 public:
    struct Config {
        int fd = STDOUT_FILENO;
    };

    PosixConsoleWriter(asio::io_context& context, Config config);

    using ConsoleWriter::write;

    platform::Async<platform::Status> write(std::string_view bytes) override;
    platform::Async<platform::Status> writeMany(std::vector<asio::const_buffer> bytes) override;

 private:
    asio::posix::stream_descriptor stdout_;
};

}  // namespace pdft::io
