#pragma once

#include "pdft/app/event/event_listener.h"
#include "pdft/notify/notify.h"

#include "pdft/core/io/console_reader.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include <asio/signal_set.hpp>

namespace pdft::app {

class AppEventListener : public EventListener, public Module {
 public:
    AppEventListener(
        asio::io_context& io,
        inject<notify::Notify, "fswatcher"> fswatcher,
        inject<io::ConsoleReader, "reader"> reader);

    AsyncResult<Event> nextEvent() override;

 private:
    AsyncResult<int> awaitSignal();

    asio::signal_set signals_;
    Arc<notify::Notify> fs_watcher_;
    Arc<io::ConsoleReader> console_reader_;
};

}  // namespace pdft::app
