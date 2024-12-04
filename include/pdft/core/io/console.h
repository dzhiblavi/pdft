#pragma once

#include "pdft/core/awaitable.h"
#include "pdft/core/io/buffer.h"

#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>

namespace pdft::io {

class Console {
 public:
    virtual ~Console() = default;

    virtual AsyncResult<void> write(std::string_view buffer) = 0;
    virtual AsyncResult<void> writeMany(std::vector<asio::const_buffer> buffers) = 0;
    virtual AsyncResult<void> read(size_t num_bytes, std::string& buffer) = 0;

    AsyncResult<void> write(OutBuffer& buffer) {
        co_return co_await this->write(buffer.view());
    }

    template <typename... Buffers>
    AsyncResult<void> writeMany(Buffers&&... buffers) {
        std::vector<asio::const_buffer> views_vec{asio::buffer(getView(buffers))...};
        co_return co_await this->writeMany(std::move(views_vec));
    }

    AsyncResult<std::string> read(size_t num_bytes) {
        std::string buffer;
        co_await this->read(num_bytes, buffer);
        co_return std::make_tuple(asio::error_code(), buffer);
    }

 private:
    static std::string_view getView(OutBuffer& buffer) {
        return buffer.view();
    }

    static std::string_view getView(std::string_view view) {
        return view;
    }
};

}  // namespace pdft::io
