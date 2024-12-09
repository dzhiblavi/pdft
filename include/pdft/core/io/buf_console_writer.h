#pragma once

#include "pdft/core/io/console_writer.h"

namespace pdft::io {

class BufConsoleWriter : public ConsoleWriter {
 public:
    explicit BufConsoleWriter(OutBuffer& buffer);

    using ConsoleWriter::write;

    AsyncStatus write(std::string_view bytes) override;
    AsyncStatus writeMany(std::vector<asio::const_buffer> bytes) override;

 private:
    OutBuffer& buffer_;
};

}  // namespace pdft::io
