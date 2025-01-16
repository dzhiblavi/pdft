#pragma once

#include "platform/event/event_emitter.h"
#include "platform/module/module.h"

#include "pdft/io/console_reader.h"

#include <asio/io_context.hpp>

namespace pdft::rt {

class KeyEventEmitter : public platform::rt::EventEmitter<char>, public platform::rt::Module {
 public:
    KeyEventEmitter(asio::io_context& io, platform::Arc<io::ConsoleReader> reader);

    void start() override;
    void stop() override;

 private:
    platform::Async<void> listenForEvents();

    asio::io_context& io_;
    platform::Arc<io::ConsoleReader> console_reader_;
    asio::cancellation_signal cancel_;
};

}  // namespace pdft::rt
