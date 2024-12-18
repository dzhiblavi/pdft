#pragma once

#include "pdft/event/event_emitter.h"
#include "pdft/io/console_reader.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include <asio/io_context.hpp>

namespace pdft::event {

class KeyEventEmitter : public EventEmitter<char>, public Module {
 public:
    KeyEventEmitter(asio::io_context& io, inject<io::ConsoleReader, "reader"> reader);

    void start();
    void stop() override;

 private:
    Async<void> listenForEvents();

    asio::io_context& io_;
    Arc<io::ConsoleReader> console_reader_;
    asio::cancellation_signal cancel_;
};

}  // namespace pdft::event
