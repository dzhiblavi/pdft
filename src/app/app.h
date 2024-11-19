#pragma once

#include "event_queue.h"
#include "keyboard_consumer.h"
#include "pdft/controller/controller.h"
#include "pdft/core/io/console.h"
#include "pdft/core/io/termctl.h"
#include "pdft/core/terminal/apc_executor.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/media/pdf/context.h"
#include "pdft/state/document_storage.h"
#include "pdft/state/state.h"
#include "pdft/view/image/display.h"
#include "pdft/view/renderer.h"

#include <asio/awaitable.hpp>

namespace app {

class Application {
 public:
    Application(asio::io_context& context) : context_(context), event_queue_(context, 10) {}

    void setup();
    asio::awaitable<void> run(std::span<const char*> paths);

 private:
    asio::awaitable<void> prepareTerminal();
    void loadPaths(std::span<const char*> paths);
    void startEventsConsumption();
    asio::awaitable<void> runEventLoop();
    asio::awaitable<void> cleanup();

    asio::io_context& context_;

    std::shared_ptr<pdft::media::pdf::Context> pdf_context_;
    std::shared_ptr<pdft::core::io::TermControl> term_control_;
    std::shared_ptr<pdft::core::terminal::Terminal> terminal_;
    std::shared_ptr<pdft::core::terminal::APCExecutor> executor_;
    std::shared_ptr<pdft::core::io::Console> console_;

    // View
    std::shared_ptr<pdft::view::image::Display> image_display_;
    std::shared_ptr<pdft::view::Renderer> renderer_;

    // State
    std::shared_ptr<pdft::state::DocumentStorage> document_storage_;
    std::shared_ptr<pdft::state::State> state_;

    // Controller
    std::shared_ptr<pdft::controller::Controller> controller_;

    // Events
    EventQueue event_queue_;
    std::shared_ptr<pdft::notify::Notify> fs_watcher_;
    std::shared_ptr<KeyboardConsumer> keyboard_consumer_;
};

}  // namespace app
