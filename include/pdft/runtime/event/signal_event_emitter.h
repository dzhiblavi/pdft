#pragma once

#include "pdft/runtime/event/event_emitter.h"
#include "pdft/runtime/module/module.h"

#include "pdft/core/awaitable.h"

#include <asio/signal_set.hpp>

namespace pdft::rt {

class SignalEventEmitter : public EventEmitter<int>, public rt::Module {
 public:
    struct Config {
        std::vector<int> signals = {};
    };

    SignalEventEmitter(Config config, asio::io_context& io);

    void start() override;
    void stop() override;

 private:
    Async<void> listenForEvents();

    asio::io_context& io_;
    asio::signal_set signals_;
    asio::cancellation_signal cancel_;
};

}  // namespace pdft::rt
