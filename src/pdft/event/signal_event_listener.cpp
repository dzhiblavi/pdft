#include "pdft/event/signal_event_listener.h"

#include "pdft/core/module/register.h"

#include <asio/bind_cancellation_slot.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::event {

SignalEventListener::SignalEventListener(asio::io_context& io)
    : io_(io)
    , signals_(io, SIGINT, SIGWINCH) {}

void SignalEventListener::start() {
    asio::co_spawn(
        io_, listenForEvents(), asio::bind_cancellation_slot(cancel_.slot(), asio::detached));
}

void SignalEventListener::stop() {
    cancel_.emit(asio::cancellation_type_t::terminal);
}

Async<void> SignalEventListener::listenForEvents() {
    auto state = co_await asio::this_coro::cancellation_state;

    while (!state.cancelled()) {
        auto [ec, signal] = co_await signals_.async_wait(AsioCoroAsTuple);

        if (!ec) {
            emit(signal);
        } else {
            log::err(this, "error when receiving a signal: ", ec);
        }
    }
}

}  // namespace pdft::event

PDFT_REGISTER_MODULE(pdft::event::SignalEventListener);
