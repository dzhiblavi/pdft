#pragma once

#include "pdft/runtime/event/event_emitter.h"
#include "pdft/runtime/module/module.h"

#include "pdft/io/console_reader.h"

#include <asio/io_context.hpp>

namespace pdft::rt {

class KeyEventEmitter : public EventEmitter<char>, public rt::Module {
 public:
    KeyEventEmitter(asio::io_context& io, Arc<io::ConsoleReader> reader);

    void start() override;
    void stop() override;

 private:
    Async<void> listenForEvents();

    asio::io_context& io_;
    Arc<io::ConsoleReader> console_reader_;
    asio::cancellation_signal cancel_;
};

}  // namespace pdft::rt
