#pragma once

#include "config.h"

#include "pdft/app/loader/loader.h"

#include "pdft/app/view/view.h"
#include "pdft/media/loader.h"

#include "pdft/tui/display.h"
#include "pdft/tui/terminal.h"

#include "pdft/notify/notify.h"

#include "pdft/core/io/console.h"
#include "pdft/core/io/termctl.h"
#include "pdft/core/log/with_logger.h"
#include "pdft/core/rc.h"

#include <asio/awaitable.hpp>
#include <asio/signal_set.hpp>

namespace app {

class Application : public pdft::log::WithLogger {
 public:
    Application(asio::io_context& context, Config config, pdft::app::StateMode state_mode);

    asio::awaitable<void> run(const std::vector<std::string>& paths);

 private:
    void printAppInfo() const;
    void setup();
    void load(const std::vector<std::string>& paths);
    void saveState();

    asio::awaitable<void> prepareTerminal();
    asio::awaitable<void> runEventLoop();
    asio::awaitable<bool> waitForNextRender();
    asio::awaitable<void> cleanup();

    const Config config_;
    asio::io_context& context_;
    pdft::app::StateMode state_mode_;
    pdft::app::UIState ui_;
    asio::signal_set signals_;

    pdft::Arc<pdft::core::io::TermControl> term_control_;
    pdft::Arc<pdft::tui::Terminal> terminal_;
    pdft::Arc<pdft::core::io::Console> console_;
    pdft::Arc<pdft::notify::Notify> fs_watcher_;
    pdft::Arc<pdft::tui::Display> image_display_;
    pdft::Arc<pdft::media::Loader> doc_loader_;
    pdft::Arc<pdft::app::Loader> loader_;
    pdft::Arc<pdft::app::DocumentStorage> documents_;
    pdft::Arc<pdft::app::View> view_;
    pdft::Arc<pdft::app::Controller> controller_;
};

}  // namespace app
