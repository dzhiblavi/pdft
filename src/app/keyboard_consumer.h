#pragma once

#include "event_queue.h"

#include "pdft/core/io/console.h"

#include <memory>

namespace app {

class KeyboardConsumer {
 public:
    explicit KeyboardConsumer(std::shared_ptr<pdft::core::io::Console> console);

    asio::awaitable<void> consumeInto(EventQueue& queue);
    void stop();

 private:
    std::shared_ptr<pdft::core::io::Console> console_;
    bool should_stop_ = false;
};

}  // namespace app
