#include "pdft/app/event/app_event_listener.h"

#include "pdft/core/module/register.h"
#include "pdft/core/result/make/err.h"

#include "pdft/utils/visit.h"

#include <asio/experimental/awaitable_operators.hpp>

namespace pdft::app {

using namespace asio::experimental::awaitable_operators;

AppEventListener::AppEventListener(
    asio::io_context& io,
    inject<notify::Notify, "fswatcher"> fswatcher,
    inject<io::ConsoleReader, "reader"> reader)
    : signals_(io, SIGINT, SIGWINCH)
    , fs_watcher_(std::move(fswatcher))
    , console_reader_(std::move(reader)) {}

AsyncResult<Event> AppEventListener::nextEvent() {
    auto event = co_await (console_reader_->read(1) || fs_watcher_->nextEvent() || awaitSignal());

    co_return utils::visit(
        event,
        [](const Result<std::string>& input) -> Result<Event> {
            return input.transform([](auto&& s) { return s[0]; });
        },
        [](const Result<notify::Event>& fsevent) -> Result<Event> { return fsevent; },
        [](const Result<int>& signal) -> Result<Event> { return signal; });
}

pdft::AsyncResult<int> AppEventListener::awaitSignal() {
    auto [ec, signal] = co_await signals_.async_wait(asio_coro_as_tuple);

    if (ec) {
        co_return result::err(errorCodeToString(ec));
    }

    co_return result::ok(signal);
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppEventListener);
