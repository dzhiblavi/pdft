#pragma once

#include "pdft/core/awaitable.h"
#include "pdft/core/io/buffer.h"
#include "pdft/core/result/make/ok.h"

#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>

namespace pdft::io {

class Console {
 public:
    virtual ~Console() = default;

    virtual AsyncStatus write(std::string_view buffer) = 0;
    virtual AsyncStatus writeMany(std::vector<asio::const_buffer> buffers) = 0;
    virtual AsyncStatus read(size_t num_bytes, std::string& buffer) = 0;

    AsyncStatus write(OutBuffer& buffer) {
        co_return co_await this->write(buffer.view());
    }

    template <typename... Buffers>
    AsyncStatus writeMany(Buffers&&... buffers) {
        std::vector<asio::const_buffer> views_vec{asio::buffer(getView(buffers))...};
        co_return co_await this->writeMany(std::move(views_vec));
    }

    AsyncResult<std::string> read(size_t num_bytes) {
        std::string buffer;
        co_await this->read(num_bytes, buffer);
        co_return result::ok(std::move(buffer));
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
