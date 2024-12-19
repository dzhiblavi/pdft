#include "pdft/event/signal_event_emitter.h"

#include "pdft/core/module/register.h"

#include <asio/bind_cancellation_slot.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::event {

SignalEventEmitter::SignalEventEmitter(asio::io_context& io)
    : io_(io)
    , signals_(io, SIGINT, SIGWINCH) {}

void SignalEventEmitter::start() {
    asio::co_spawn(
        io_, listenForEvents(), asio::bind_cancellation_slot(cancel_.slot(), asio::detached));
}

void SignalEventEmitter::stop() {
    cancel_.emit(asio::cancellation_type_t::terminal);
}

Async<void> SignalEventEmitter::listenForEvents() {
    auto state = co_await asio::this_coro::cancellation_state;

    while (!state.cancelled()) {
        auto [ec, signal] = co_await signals_.async_wait(AsioCoroAsTuple);

        if (!ec) {
            emit(signal);
        } else {
            log::err(this, "error when receiving a signal: {}", ec);
        }
    }
}

}  // namespace pdft::event

PDFT_REGISTER_MODULE(pdft::event::SignalEventEmitter);
