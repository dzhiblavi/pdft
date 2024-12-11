#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/loader/app_loader.h"
#include "pdft/app/view/view.h"

#include "pdft/app/event/event_listener.h"

#include "pdft/tui/display.h"
#include "pdft/tui/terminal.h"

#include "pdft/core/io/console_writer.h"
#include "pdft/core/io/termctl.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"
#include "pdft/core/rc.h"

#include <asio/signal_set.hpp>

namespace pdft::app {

class Application : public Module {
 public:
    Application(
        inject<AppLoader, "loader"> loader,
        inject<EventListener, "listener"> listener,
        inject<io::ConsoleWriter, "writer"> writer,
        inject<io::TermControl, "term_ctrl"> term_control,
        inject<tui::Terminal, "terminal"> terminal,
        inject<tui::Display, "display"> display,
        inject<DocumentStorage, "docs"> documents,
        inject<Controller, "controller"> controller,
        inject<View, "view"> view);

    AsyncStatus run();

 private:
    Async<void> prepareTerminal();
    Async<void> runEventLoop();
    Async<bool> waitForNextRender();
    Async<void> cleanup();

    UIState ui_;

    Arc<AppLoader> loader_;
    Arc<EventListener> listener_;
    Arc<io::TermControl> term_control_;
    Arc<io::ConsoleWriter> console_writer_;
    Arc<tui::Terminal> terminal_;
    Arc<tui::Display> image_display_;
    Arc<DocumentStorage> documents_;
    Arc<View> view_;
    Arc<Controller> controller_;
};

}  // namespace pdft::app
