#pragma once

#include "pdft/core/awaitable.h"
#include "pdft/core/result.h"

#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>

namespace pdft::io {

class ConsoleReader {
 public:
    virtual ~ConsoleReader() = default;

    virtual Async<Status> read(size_t num_bytes, std::string& buffer) = 0;

    AsyncResult<std::string> read(size_t num_bytes) {
        std::string buffer;
        co_return co_await this->read(num_bytes, buffer) |
            result::map([&](auto) { return std::move(buffer); });
    }
};

}  // namespace pdft::io
