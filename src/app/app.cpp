#include "app.h"
#include "consume_fs_events.h"
#include "keyboard_consumer.h"
#include "resolve_apc_executor.h"
#include "resolve_display.h"
#include "setup_logging.h"

#include "pdft/core/io/posix/console.h"
#include "pdft/core/io/posix/termctl.h"
#include "pdft/core/log/log.h"
#include "pdft/core/notify/fsw/notify.h"
#include "pdft/core/terminal/ansi/terminal.h"
#include "pdft/core/trace/log.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/controller/app/controller.h"
#include "pdft/view/app/renderer.h"

#include "pdft/state/app/state.h"
#include "pdft/state/document_storage.h"

#include "pdft/media/pdf/context.h"
#include "pdft/media/pdf/document.h"

#include "pdft/utils/visit.h"
#include "setup_signal_handlers.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

extern "C" {
#include <mupdf/fitz/context.h>
}

#include <asio/awaitable.hpp>
#include <asio/experimental/channel.hpp>

namespace app {

using namespace pdft;

void Application::setup() {
    app::setupSignalHandlers();
    app::setupLogging();

    pdf_context_ = std::make_shared<media::pdf::Context>(media::pdf::Context::Builder().build());
    app::setupMupdfLogging(*pdf_context_);
    pdf_context_->registerDocumentHandlers();

    term_control_ = std::make_shared<core::io::posix::TermControl>();
    terminal_ = std::make_shared<core::terminal::ansi::Terminal>();
    executor_ = resolveAPCExecutor();
    console_ = std::make_shared<core::io::posix::Console>(
        context_, ::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));

    keyboard_consumer_ = std::make_shared<KeyboardConsumer>(console_);
    document_storage_ = std::make_shared<state::DocumentStorage>(pdf_context_);
    image_display_ = resolveDisplay(terminal_, executor_);
    renderer_ = std::make_shared<view::app::Renderer>(document_storage_, image_display_, terminal_);
    state_ = std::make_shared<state::app::State>(document_storage_, renderer_);
    controller_ = std::make_shared<controller::app::Controller>(state_, document_storage_);

    if (config_.system.enable_filesystem_watching) {
        fs_watcher_ = std::make_shared<pdft::notify::fsw::Notify<asio::io_context::executor_type>>(
            context_.get_executor());
    }
}

asio::awaitable<void> Application::run(const std::vector<std::string>& paths) {
    co_await prepareTerminal();
    loadPaths(paths);
    startEventsConsumption();
    co_await runEventLoop();
    co_await cleanup();
}

asio::awaitable<void> Application::prepareTerminal() {
    // Configure terminal
    term_control_->ioControl(core::io::ControlType::Input, {.canonical = false, .echo = false});

    // Clear the screen
    core::io::OutBuffer init_buffer;
    terminal_->clear(init_buffer);
    co_await console_->write(init_buffer.view());
}

void Application::loadPaths(const std::vector<std::string>& paths) {
    for (auto path : paths) {
        if (!std::filesystem::exists(path)) {
            core::log::global()->info("path {} does not exist, skipping", path.c_str());
            continue;
        }

        path = std::filesystem::absolute(path);

        auto doc_id =
            document_storage_->addDocument(media::pdf::Document::open(*pdf_context_, path));
        state_->addDocument(doc_id);

        if (config_.system.enable_filesystem_watching) {
            fs_watcher_->add(path);
        }

        core::log::global()->info("loaded path {}", path.c_str());
    }
}

void Application::startEventsConsumption() {
    // Start event consumption
    asio::co_spawn(context_, keyboard_consumer_->consumeInto(event_queue_), asio::detached);

    if (config_.system.enable_filesystem_watching) {
        asio::co_spawn(context_, consumeFsEvents(event_queue_, *fs_watcher_), asio::detached);
        core::log::global()->info("Started filesystem watcher");
    } else {
        core::log::global()->info(
            "Filesystem watching has been disabled by config (.system.enable_filesystem_watching)");
    }
}

asio::awaitable<void> Application::runEventLoop() {
    // Prepare variables reused in the event loop
    auto buffer = core::io::OutBuffer();
    auto term_size = core::terminal::getTerminalDimensions();
    auto screen = view::IRect{
        .x = 0,
        .y = 0,
        .width = term_size.columns,
        .height = term_size.rows,
    };

    for (;;) {
        core::log::global()->info("========== NEW FRAME ===========");

        auto tracer =
            core::trace::getGlobalPool().getTracer(core::trace::stackTraceLogger(), "display_one");

        core::io::clear(buffer);
        terminal_->clear(buffer);

        {
            auto _ = tracer.addChild("render");
            state_->render(screen, buffer);
        }

        {
            auto _ = tracer.addChild("flush");
            co_await console_->write(buffer.view());
        }

        bool quit = co_await [&]() -> asio::awaitable<bool> {
            for (;;) {
                auto event = co_await event_queue_.async_receive(asio::use_awaitable);

                auto action = utils::visit(
                    event,
                    [&](KeyPressEvent event) { return controller_->onKeyEvent(event.key); },
                    [&](notify::Event event) { return controller_->onFilesystemEvent(event); });

                using enum controller::Action;

                if (action == Quit) {
                    co_return true;
                }

                if (action == Redraw) {
                    co_return false;
                }
            }
        }();

        if (quit) {
            break;
        }
    }
}

asio::awaitable<void> Application::cleanup() {
    auto cleanup = core::trace::getGlobalPool().getTracer(
        core::trace::stackTraceLogger<std::chrono::microseconds>(), "cleanup");

    core::io::OutBuffer buffer;
    {
        auto _ = cleanup.addChild("term_clear");
        terminal_->clear(buffer);
        co_await console_->write(buffer.view());
    }

    {
        auto _ = cleanup.addChild("keyboard_stop");
        keyboard_consumer_->stop();
    }

    if (config_.system.enable_filesystem_watching) {
        auto _ = cleanup.addChild("notify_stop");
        fs_watcher_->stop();
    }

    core::log::global()->info(
        "total stack tracer allocations: {}", core::trace::getGlobalPool().numAllocations());
}

}  // namespace app
