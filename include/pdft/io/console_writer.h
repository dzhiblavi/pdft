#pragma once

#include "pdft/io/buffer.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/result/type/status.h"

#include <asio/buffer.hpp>

namespace pdft::io {

class ConsoleWriter {
 public:
    virtual ~ConsoleWriter() = default;

    virtual Async<Status> write(std::string_view buffer) = 0;
    virtual Async<Status> writeMany(std::vector<asio::const_buffer> buffers) = 0;

    Async<Status> write(OutBuffer& buffer) {
        co_return co_await this->write(buffer.view());
    }

    template <typename... Buffers>
    Async<Status> writeMany(Buffers&&... buffers) {
        std::vector<asio::const_buffer> views_vec{
            asio::buffer(getView(std::forward<Buffers>(buffers)))...};

        co_return co_await this->writeMany(std::move(views_vec));
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
