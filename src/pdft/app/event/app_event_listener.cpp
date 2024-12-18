#include "pdft/app/event/impl/app_event_listener.h"

#include "pdft/core/module/register.h"
#include "pdft/core/result/combine/map.h"
#include "pdft/core/result/make/err.h"

#include "pdft/utils/visit.h"

#include <asio/bind_cancellation_slot.hpp>
#include <asio/detached.hpp>
#include <asio/experimental/awaitable_operators.hpp>

namespace pdft::app {

using namespace asio::experimental::awaitable_operators;

AppEventListener::AppEventListener(
    Config config,
    asio::io_context& io,
    inject<notify::Notify, "fswatcher"> fswatcher,
    inject<io::ConsoleReader, "reader"> reader)
    : io_(io)
    , signals_(io, SIGINT, SIGWINCH)
    , fs_watcher_(std::move(fswatcher))
    , console_reader_(std::move(reader))
    , channel_(io, config.max_event_buffer_size) {}

void AppEventListener::start() {
    asio::co_spawn(
        io_, listenForEvents(), asio::bind_cancellation_slot(cancel_.slot(), asio::detached));
}

void AppEventListener::stop() {
    cancel_.emit(asio::cancellation_type_t::terminal);
}

AsyncResult<Event> AppEventListener::nextEvent() {
    auto [ec, event] = co_await channel_.async_receive(AsioCoroAsTuple);

    if (ec) {
        co_return result::err(errorCodeToString(ec));
    }

    co_return std::move(event);
}

Async<void> AppEventListener::listenForEvents() {
    auto state = co_await asio::this_coro::cancellation_state;

    while (!state.cancelled()) {
        co_await channel_.async_send({}, co_await getNextEvent());
    }
}

AsyncResult<Event> AppEventListener::getNextEvent() {
    auto event = utils::visit(
        co_await (console_reader_->read(1) || fs_watcher_->nextEvent() || awaitSignal()),
        [](const Result<std::string>& input) -> Result<EventType> {
            return input.transform([](auto&& s) { return s[0]; });
        },
        [](const Result<notify::Event>& fsevent) -> Result<EventType> { return fsevent; },
        [](const Result<int>& signal) -> Result<EventType> { return signal; });

    co_return event | result::map([](EventType evt) {
        return Event{
            .timestamp = SteadyClockType::now(),
            .event = std::move(evt),
        };
    });
}

pdft::AsyncResult<int> AppEventListener::awaitSignal() {
    auto [ec, signal] = co_await signals_.async_wait(AsioCoroAsTuple);

    if (ec) {
        co_return result::err(errorCodeToString(ec));
    }

    co_return result::ok(signal);
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppEventListener);
