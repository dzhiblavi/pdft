#pragma once

#include "config.h"
#include "state.h"

#include "pdft/app/controller/controller.h"
#include "pdft/app/view/view.h"

#include "pdft/media/loader.h"
#include "pdft/tui/display/display.h"
#include "pdft/tui/terminal/terminal.h"

#include "pdft/core/io/console.h"
#include "pdft/core/io/termctl.h"
#include "pdft/core/log/with_logger.h"
#include "pdft/notify/notify.h"

#include <asio/awaitable.hpp>
#include <asio/signal_set.hpp>

namespace app {

class Application : public pdft::core::log::WithLogger {
 public:
    Application(asio::io_context& context, Config config, StateMode state_mode);

    asio::awaitable<void> run(const std::vector<std::string>& paths);

 private:
    void printAppInfo() const;
    void setup();
    void restoreState();
    void saveState();
    bool isOpened(const std::string& abs_path) const;
    asio::awaitable<void> prepareTerminal();
    void loadPaths(const std::vector<std::string>& paths);
    asio::awaitable<void> runEventLoop();
    asio::awaitable<void> render(
        pdft::core::io::OutBuffer& control_buffer, pdft::core::io::OutBuffer& display_buffer);
    asio::awaitable<bool> waitForNextRender();
    asio::awaitable<void> cleanup();

    asio::io_context& context_;
    Config config_;
    StateMode state_mode_;

    std::shared_ptr<pdft::core::io::TermControl> term_control_;
    std::shared_ptr<pdft::tui::terminal::Terminal> terminal_;
    std::shared_ptr<pdft::core::io::Console> console_;
    std::shared_ptr<pdft::notify::Notify> fs_watcher_;
    std::shared_ptr<pdft::tui::display::Display> image_display_;
    std::shared_ptr<pdft::media::Loader> doc_loader_;

    pdft::app::state::UIState ui_;
    std::shared_ptr<pdft::app::state::DocumentStorage> documents_;
    pdft::tui::display::State display_;

    std::shared_ptr<pdft::app::view::View> view_;
    std::shared_ptr<pdft::app::controller::Controller> controller_;

    asio::signal_set signals_;
};

}  // namespace app
