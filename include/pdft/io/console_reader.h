#pragma once

#include "platform/core/async.h"
#include "platform/core/result.h"

#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>

namespace pdft::io {

class ConsoleReader {
 public:
    virtual ~ConsoleReader() = default;

    virtual platform::Async<platform::Status> read(size_t num_bytes, std::string& buffer) = 0;

    platform::Async<platform::Result<std::string>> read(size_t num_bytes) {
        std::string buffer;
        co_return co_await this->read(num_bytes, buffer) |
            result::map([&](auto) { return std::move(buffer); });
    }
};

}  // namespace pdft::io
