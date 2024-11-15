#pragma once

#include <asio/awaitable.hpp>

namespace pdft::core::io {

class Console {
 public:
    virtual ~Console() = default;

    virtual asio::awaitable<void> write(std::string_view bytes) = 0;
    virtual asio::awaitable<std::string> read(size_t num_bytes) = 0;
};

}  // namespace pdft::core::io
