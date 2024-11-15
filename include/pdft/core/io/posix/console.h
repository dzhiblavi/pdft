#pragma once

#include "pdft/core/io/console.h"

#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>

namespace pdft::core::io::posix {

class Console : public io::Console {
 public:
    explicit Console(asio::io_context& context, int stdin_fd, int stdout_fd);

    asio::awaitable<void> write(std::string_view bytes) override;
    asio::awaitable<std::string> read(size_t num_bytes) override;

 private:
    asio::posix::stream_descriptor stdin_;
    asio::posix::stream_descriptor stdout_;
};

}  // namespace pdft::core::io::posix
