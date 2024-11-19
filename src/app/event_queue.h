#pragma once

#include "pdft/core/notify/notify.h"

#include <asio/experimental/channel.hpp>
#include <asio/io_context.hpp>

namespace app {

struct KeyPressEvent {
    char key;
};

using Event = std::variant<KeyPressEvent, pdft::notify::Event>;

using EventQueue =
    asio::experimental::channel<asio::io_context::executor_type, void(asio::error_code, Event)>;

}  // namespace app
