#include "app.h"
#include "logging.h"
#include "resolve_apc_executor.h"
#include "resolve_display.h"
#include "setup_signal_handlers.h"
#include "version.h"

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

#include "pdft/utils/build_info.h"
#include "pdft/utils/visit.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <magic_enum.hpp>

extern "C" {
#include <mupdf/fitz/context.h>
}

#include <asio/awaitable.hpp>
#include <asio/experimental/awaitable_operators.hpp>
#include <asio/experimental/channel.hpp>

namespace app {

using namespace pdft;
using namespace asio::experimental::awaitable_operators;

Application::Application(asio::io_context& context, Config config)
    : context_(context)
    , config_(config) {
    setup();
}

void Application::setup() {
    app::setupSignalHandlers();
    app::setupLogging(config_.logging);

    pdf_context_ = std::make_shared<media::pdf::Context>(media::pdf::Context::Builder().build());
    app::setupMupdfLogging(*pdf_context_);
    pdf_context_->registerDocumentHandlers();

    term_control_ = std::make_shared<core::io::posix::TermControl>();
    terminal_ = std::make_shared<core::terminal::ansi::Terminal>();
    console_ = std::make_shared<core::io::posix::Console>(
        context_, ::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));
    executor_ = resolveAPCExecutor(console_);

    document_storage_ = std::make_shared<state::DocumentStorage>(pdf_context_);
    image_display_ = resolveDisplay(terminal_, executor_, console_);
    renderer_ = std::make_shared<view::app::Renderer>(document_storage_, image_display_, terminal_);
    state_ = std::make_shared<state::app::State>(document_storage_, renderer_);
    controller_ = std::make_shared<controller::app::Controller>(
        config_.controller, state_, document_storage_);

    if (config_.system.enable_filesystem_watching) {
        fs_watcher_ = std::make_shared<pdft::notify::fsw::Notify<asio::io_context::executor_type>>(
            context_.get_executor());
    }
}

asio::awaitable<void> Application::run(const std::vector<std::string>& paths) {
    co_await prepareTerminal();
    loadPaths(paths);
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
            log::info("path {} does not exist, skipping", path.c_str());
            continue;
        }

        path = std::filesystem::absolute(path);

        auto doc_id =
            document_storage_->addDocument(media::pdf::Document::open(*pdf_context_, path));
        state_->addDocument(doc_id);

        if (config_.system.enable_filesystem_watching) {
            fs_watcher_->add(path);
        }

        log::info("loaded path {}", path.c_str());
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

    using namespace utils;
    using magic_enum::enum_name;

    log::info("PDFT {}", getFullVersionString());
    log::info(
        "Platform={}, ASAN={}, TSAN={}, UBSAN={}",
        enum_name(getCurrentPlatform()),
        enum_name(isBuiltWithASAN()),
        enum_name(isBuiltWithTSAN()),
        enum_name(isBuiltWithUBSAN()));

    log::info("Display size is {}x{} cells", term_size.columns, term_size.rows);

    for (;;) {
        log::info("========== NEW FRAME ===========");

        auto tracer =
            core::trace::getGlobalPool().getTracer(core::trace::stackTraceLogger(), "display_one");

        core::io::clear(buffer);
        terminal_->clear(buffer);
        image_display_->clear(buffer);

        {
            auto _ = tracer.addChild("render");
            co_await state_->render(screen, buffer);
        }

        {
            auto _ = tracer.addChild("flush");
            co_await console_->write(buffer.view());
        }

        bool quit = co_await [&]() -> asio::awaitable<bool> {
            for (;;) {
                auto event = co_await (console_->read(1) || fs_watcher_->nextEvent());

                auto action = utils::visit(
                    event,
                    [&](std::string event) { return controller_->onKeyEvent(event[0]); },
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

    if (config_.system.enable_filesystem_watching) {
        auto _ = cleanup.addChild("notify_stop");
        fs_watcher_->stop();
    }

    log::info("total stack tracer allocations: {}", core::trace::getGlobalPool().numAllocations());
}

}  // namespace app
