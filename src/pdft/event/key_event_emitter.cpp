#include "pdft/event/key_event_emitter.h"

#include "pdft/core/module/register.h"

#include <asio/bind_cancellation_slot.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::event {

KeyEventEmitter::KeyEventEmitter(asio::io_context& io, Arc<io::ConsoleReader> reader)
    : io_(io)
    , console_reader_(std::move(reader)) {}

void KeyEventEmitter::start() {
    asio::co_spawn(
        io_, listenForEvents(), asio::bind_cancellation_slot(cancel_.slot(), asio::detached));
}

void KeyEventEmitter::stop() {
    cancel_.emit(asio::cancellation_type_t::terminal);
}

Async<void> KeyEventEmitter::listenForEvents() {
    auto state = co_await asio::this_coro::cancellation_state;

    while (!state.cancelled()) {
        auto key = co_await console_reader_->read(1);

        if (key) {
            emit((*key)[0]);
        } else {
            log::err(this, "error when reading input from console: {}", key.error());
        }
    }
}

}  // namespace pdft::event

PDFT_REGISTER_MODULE(pdft::event::KeyEventEmitter);
