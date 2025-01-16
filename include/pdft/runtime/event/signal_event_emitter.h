#pragma once

#include "platform/event/event_emitter.h"
#include "platform/module/module.h"

#include "platform/core/async.h"

#include <asio/signal_set.hpp>

namespace pdft::rt {

class SignalEventEmitter : public platform::rt::EventEmitter<int>, public platform::rt::Module {
 public:
    struct Config {
        std::vector<int> signals = {};
    };

    SignalEventEmitter(Config config, asio::io_context& io);

    void start() override;
    void stop() override;

 private:
    platform::Async<void> listenForEvents();

    asio::io_context& io_;
    asio::signal_set signals_;
    asio::cancellation_signal cancel_;
};

}  // namespace pdft::rt
