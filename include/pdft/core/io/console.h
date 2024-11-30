#pragma once

#include "pdft/core/io/buffer.h"

#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>

namespace pdft::core::io {

class Console {
 public:
    virtual ~Console() = default;

    virtual asio::awaitable<void> write(std::string_view buffer) = 0;
    virtual asio::awaitable<void> writeMany(std::vector<asio::const_buffer> buffers) = 0;

    asio::awaitable<void> write(OutBuffer& buffer) {
        co_await this->write(buffer.view());
    }

    template <typename... Buffers>
    asio::awaitable<void> writeMany(Buffers&&... buffers) {
        std::vector<asio::const_buffer> views_vec{asio::buffer(getView(buffers))...};
        co_await this->writeMany(std::move(views_vec));
    }

    virtual asio::awaitable<void> read(size_t num_bytes, std::string& buffer) = 0;

    asio::awaitable<std::string> read(size_t num_bytes) {
        std::string buffer;
        co_await this->read(num_bytes, buffer);
        co_return buffer;
    }

 private:
    static std::string_view getView(OutBuffer& buffer) {
        return buffer.view();
    }

    static std::string_view getView(std::string_view view) {
        return view;
    }
};

}  // namespace pdft::core::io
