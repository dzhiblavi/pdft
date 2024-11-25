#pragma once

#include "config.h"
#include "pdft/controller/controller.h"
#include "pdft/core/io/console.h"
#include "pdft/core/io/termctl.h"
#include "pdft/notify/notify.h"
#include "pdft/state/document_storage.h"
#include "pdft/state/state.h"
#include "pdft/tui/display/display.h"
#include "pdft/tui/terminal/terminal.h"
#include "pdft/view/renderer.h"

#include <asio/awaitable.hpp>

namespace app {

class Application {
 public:
    Application(asio::io_context& context, Config config);

    asio::awaitable<void> run(const std::vector<std::string>& paths);

 private:
    void setup();
    asio::awaitable<void> prepareTerminal();
    void loadPaths(const std::vector<std::string>& paths);
    asio::awaitable<void> runEventLoop();
    asio::awaitable<void> cleanup();

    asio::io_context& context_;
    Config config_;

    std::shared_ptr<pdft::core::io::TermControl> term_control_;
    std::shared_ptr<pdft::tui::terminal::Terminal> terminal_;
    std::shared_ptr<pdft::core::io::Console> console_;
    std::shared_ptr<pdft::notify::Notify> fs_watcher_;

    // View
    std::shared_ptr<pdft::tui::display::Display> image_display_;
    std::shared_ptr<pdft::view::Renderer> renderer_;

    // State
    pdft::tui::terminal::Dimensions term_size_ = pdft::tui::terminal::getTerminalDimensions();
    std::shared_ptr<pdft::state::DocumentStorage> document_storage_;
    std::shared_ptr<pdft::state::State> state_;

    // Controller
    std::shared_ptr<pdft::controller::Controller> controller_;
};

}  // namespace app
