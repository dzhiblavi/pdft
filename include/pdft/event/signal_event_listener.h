#pragma once

#include "pdft/event/event_emitter.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/module/module.h"

#include <asio/signal_set.hpp>

namespace pdft::event {

class SignalEventListener : public EventEmitter<int>, public Module {
 public:
    explicit SignalEventListener(asio::io_context& io);

    void start();
    void stop() override;

 private:
    Async<void> listenForEvents();

    asio::io_context& io_;
    asio::signal_set signals_;
    asio::cancellation_signal cancel_;
};

}  // namespace pdft::event
