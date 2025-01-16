#include "pdft/runtime/event/signal_event_emitter.h"

#include "pdft/core/asio_as_tuple.h"
#include "pdft/core/log/common_adapters.h"  // IWYU pragma: keep

#include "platform/module/register.h"

#include <asio/as_tuple.hpp>
#include <asio/bind_cancellation_slot.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::rt {

SignalEventEmitter::SignalEventEmitter(Config config, asio::io_context& io)
    : io_(io)
    , signals_(io) {
    for (int signal : config.signals) {
        // Don't care about the errors thrown
        signals_.add(signal);
    }
}

void SignalEventEmitter::start() {
    asio::co_spawn(
        io_, listenForEvents(), asio::bind_cancellation_slot(cancel_.slot(), asio::detached));
}

void SignalEventEmitter::stop() {
    cancel_.emit(asio::cancellation_type_t::terminal);
}

platform::Async<void> SignalEventEmitter::listenForEvents() {
    auto state = co_await asio::this_coro::cancellation_state;

    while (!state.cancelled()) {
        auto [ec, signal] = co_await signals_.async_wait(AsioCoroAsTuple);

        if (!ec) {
            emit(signal);
        } else {
            platform::log::err(this, "error when receiving a signal: {}", ec);
        }
    }
}

}  // namespace pdft::rt

PLATFORM_REGISTER_MODULE(pdft::rt::SignalEventEmitter);
