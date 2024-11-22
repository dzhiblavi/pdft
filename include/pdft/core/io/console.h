#pragma once

#include "pdft/core/io/buffer.h"

#include <asio/awaitable.hpp>

namespace pdft::core::io {

class Console {
 public:
    virtual ~Console() = default;

    virtual asio::awaitable<void> write(std::string_view bytes) = 0;
    virtual asio::awaitable<void> read(size_t num_bytes, std::string& buffer) = 0;

    asio::awaitable<void> write(OutBuffer& buffer) {
        co_await this->write(buffer.view());
        io::clear(buffer);
    }

    asio::awaitable<std::string> read(size_t num_bytes) {
        std::string buffer;
        co_await this->read(num_bytes, buffer);
        co_return buffer;
    }
};

}  // namespace pdft::core::io
