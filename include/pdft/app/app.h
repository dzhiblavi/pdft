#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/loader/loader.h"
#include "pdft/app/view/view.h"

#include "pdft/tui/display.h"
#include "pdft/tui/terminal.h"

#include "pdft/notify/notify.h"

#include "pdft/core/io/console_reader.h"
#include "pdft/core/io/console_writer.h"
#include "pdft/core/io/termctl.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"
#include "pdft/core/rc.h"

#include <asio/signal_set.hpp>

namespace pdft::app {

class Application : public Module {
 public:
    struct Config {
        std::optional<std::string> state_path;
    };

    Application(
        asio::io_context& context,
        Config config,
        inject<notify::Notify, "fswatcher"> fswatcher,
        inject<io::ConsoleWriter, "writer"> writer,
        inject<io::ConsoleReader, "reader"> reader,
        inject<io::TermControl, "term_ctrl"> term_control,
        inject<tui::Terminal, "terminal"> terminal,
        inject<tui::Display, "display"> display,
        inject<DocumentStorage, "docs"> documents,
        inject<Loader, "loader"> loader,
        inject<Controller, "controller"> controller,
        inject<View, "view"> view);

    asio::awaitable<void> run(const std::vector<std::string>& paths);

 private:
    void load(const std::vector<std::string>& paths);
    void saveState();

    AsyncResult<int> awaitSignal();
    Async<void> prepareTerminal();
    Async<void> runEventLoop();
    Async<bool> waitForNextRender();
    Async<void> cleanup();
    Result<ControllerAction> handleSignal(int signal);

    const Config config_;
    UIState ui_;
    asio::signal_set signals_;

    Arc<notify::Notify> fs_watcher_;
    Arc<io::TermControl> term_control_;
    Arc<io::ConsoleWriter> console_writer_;
    Arc<io::ConsoleReader> console_reader_;
    Arc<tui::Terminal> terminal_;
    Arc<tui::Display> image_display_;
    Arc<Loader> loader_;
    Arc<DocumentStorage> documents_;
    Arc<View> view_;
    Arc<Controller> controller_;
};

}  // namespace pdft::app
