#pragma once

#include "pdft/app/event/event_listener.h"
#include "pdft/notify/notify.h"

#include "pdft/io/console_reader.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include <asio/experimental/channel.hpp>
#include <asio/signal_set.hpp>

namespace pdft::app {

class AppEventListener : public EventListener, public Module {
 public:
    struct Config {
        static constexpr size_t DefaultMaxEventBufferSize = 10;
        size_t max_event_buffer_size = DefaultMaxEventBufferSize;
    };

    AppEventListener(
        Config config,
        asio::io_context& io,
        inject<notify::Notify, "fswatcher"> fswatcher,
        inject<io::ConsoleReader, "reader"> reader);

    void start();

    AsyncResult<Event> nextEvent() override;
    void stop() override;

 private:
    AsyncResult<Event> getNextEvent();
    AsyncResult<int> awaitSignal();
    Async<void> listenForEvents();

    asio::io_context& io_;
    asio::signal_set signals_;
    Arc<notify::Notify> fs_watcher_;
    Arc<io::ConsoleReader> console_reader_;
    asio::experimental::channel<void(asio::error_code, Result<Event>)> channel_;
    asio::cancellation_signal cancel_;
};

}  // namespace pdft::app
