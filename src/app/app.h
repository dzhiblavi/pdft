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

#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/signal_set.hpp>

namespace app {

using pdft::inject;

class Application : public pdft::Module {
 public:
    struct Config {
        std::optional<std::string> state_path;
    };

    Application(
        asio::io_context& context,
        Config config,
        inject<pdft::notify::Notify, "fswatcher"> fswatcher,
        inject<pdft::io::ConsoleWriter, "writer"> writer,
        inject<pdft::io::ConsoleReader, "reader"> reader,
        inject<pdft::io::TermControl, "term_ctrl"> term_control,
        inject<pdft::tui::Terminal, "terminal"> terminal,
        inject<pdft::app::DocumentStorage, "docs"> documents,
        inject<pdft::app::Loader, "loader"> loader,
        inject<pdft::tui::Display, "display"> display,
        inject<pdft::app::Controller, "controller"> controller,
        inject<pdft::app::View, "view"> view);

    asio::awaitable<void> run(const std::vector<std::string>& paths);

 private:
    void load(const std::vector<std::string>& paths);
    void saveState();

    pdft::AsyncResult<int> awaitSignal();
    asio::awaitable<void> prepareTerminal();
    asio::awaitable<void> runEventLoop();
    asio::awaitable<bool> waitForNextRender();
    asio::awaitable<void> cleanup();

    const Config config_;
    pdft::app::UIState ui_;
    asio::signal_set signals_;

    pdft::Arc<pdft::notify::Notify> fs_watcher_;
    pdft::Arc<pdft::io::TermControl> term_control_;
    pdft::Arc<pdft::io::ConsoleWriter> console_writer_;
    pdft::Arc<pdft::io::ConsoleReader> console_reader_;
    pdft::Arc<pdft::tui::Terminal> terminal_;
    pdft::Arc<pdft::tui::Display> image_display_;
    pdft::Arc<pdft::app::Loader> loader_;
    pdft::Arc<pdft::app::DocumentStorage> documents_;
    pdft::Arc<pdft::app::View> view_;
    pdft::Arc<pdft::app::Controller> controller_;
};

}  // namespace app
