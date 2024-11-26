#include "app.h"
#include "logging.h"
#include "resolve_display.h"
#include "version.h"

#include "pdft/core/io/posix/console.h"
#include "pdft/core/io/posix/termctl.h"
#include "pdft/core/log/log.h"
#include "pdft/core/trace/make_stack_tracer_logger.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/tui/terminal/ansi/terminal.h"

#include "pdft/controller/app/controller.h"
#include "pdft/notify/fsw/notify.h"
#include "pdft/view/app/renderer.h"

#include "pdft/state/app/state.h"
#include "pdft/state/document_storage.h"

#include "pdft/media/mupdf/context.h"
#include "pdft/media/mupdf/document.h"

#include "pdft/utils/build_info.h"
#include "pdft/utils/visit.h"

#include <asio/experimental/awaitable_operators.hpp>
#include <asio/io_context.hpp>

#include <magic_enum.hpp>

namespace app {

using namespace pdft;
using namespace asio::experimental::awaitable_operators;

namespace {

controller::Action handleSignal(int signal) {
    switch (signal) {
        using enum controller::Action;

        case SIGWINCH:
            log::info("received SIGWINCH: terminal resized, redrawing");
            return Redraw;

        case SIGINT:
            log::info("received SIGINT: exiting");
            return Quit;

        case SIGSEGV:
            pdft::core::fail("received SIGSEGV");

        default:
            pdft::core::fail(std::format("received unknown signal ({})", signal).c_str());
    }
}

}  // namespace

Application::Application(asio::io_context& context, Config config)
    : context_(context)
    , config_(config)
    , signals_(context, SIGWINCH, SIGINT, SIGSEGV) {
    setup();
}

void Application::setup() {
    app::setupMupdfLogging(media::mupdf::thisThreadContext());
    media::mupdf::thisThreadContext().registerDocumentHandlers();

    term_control_ = std::make_shared<core::io::posix::TermControl>();
    terminal_ = std::make_shared<tui::terminal::ansi::Terminal>();
    console_ = std::make_shared<core::io::posix::Console>(
        context_, ::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));

    document_storage_ = std::make_shared<state::DocumentStorage>();
    image_display_ = resolveDisplay(console_);
    renderer_ = std::make_shared<view::app::Renderer>(document_storage_, image_display_);
    state_ = std::make_shared<state::app::State>(
        state::app::State::Config{.image_properties = config_.image}, document_storage_, renderer_);
    controller_ = std::make_shared<controller::app::Controller>(
        config_.controller, state_, document_storage_);

    if (config_.system.enable_filesystem_watching) {
        fs_watcher_ = std::make_shared<pdft::notify::fsw::Notify<asio::io_context::executor_type>>(
            context_.get_executor());
    }
}

void Application::printAppInfo() const {
    using namespace utils;
    using magic_enum::enum_name;

    log::info(this, "PDFT {}", getFullVersionString());

    log::info(
        this,
        "Platform={}, ASAN={}, TSAN={}, UBSAN={}",
        enum_name(getCurrentPlatform()),
        enum_name(isBuiltWithASAN()),
        enum_name(isBuiltWithTSAN()),
        enum_name(isBuiltWithUBSAN()));
}

asio::awaitable<void> Application::run(const std::vector<std::string>& paths) {
    printAppInfo();
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
    co_await console_->write(init_buffer);
}

void Application::loadPaths(const std::vector<std::string>& paths) {
    for (auto path : paths) {
        PDFT_VERIFY(std::filesystem::exists(path));
        path = std::filesystem::absolute(path);

        auto document = media::mupdf::Document::open(media::mupdf::thisThreadContext(), path);
        auto doc_id = document_storage_->addDocument(
            std::unique_ptr<media::Document>(new media::mupdf::Document(std::move(document))));

        state_->addDocument(doc_id);

        if (config_.system.enable_filesystem_watching) {
            fs_watcher_->add(path);
        }

        log::info("loaded path {}", path.c_str());
    }
}

asio::awaitable<void> Application::runEventLoop() {
    core::io::OutBuffer buffer;

    for (;;) {
        co_await render(buffer);

        bool should_quit = co_await waitForNextRender();

        if (should_quit) {
            break;
        }
    }
}

asio::awaitable<void> Application::render(core::io::OutBuffer& buffer) {
    log::debug(this, "========== NEW FRAME ===========");

    auto tracer = core::trace::getGlobalPool().getTracer("display_one");
    auto screen = ftxui::Screen::Create(ftxui::Dimension::Full());
    auto ctx = tui::Context(*terminal_, buffer);
    ctx.setTracer(tracer);

    {
        auto _ = ctx.push("render");
        tui::Render(screen, state_->render(ctx));
    }
    {
        auto _ = ctx.push("clear_screen_flush");
        core::io::OutBuffer clear_buf;
        core::io::clear(clear_buf);
        terminal_->clear(clear_buf);
        image_display_->clear(clear_buf);
        co_await console_->write(clear_buf.view());
    }
    {
        auto _ = ctx.push("flush_tui");
        co_await console_->write(screen.ToString());
    }
    {
        auto _ = ctx.push("flush_images");
        co_await console_->write(buffer);
    }

    tracer.finalize();
    core::trace::logStackTrace(tracer.getInfo());
}

asio::awaitable<bool> Application::waitForNextRender() {
    for (;;) {
        using enum controller::Action;

        auto event = co_await (
            console_->read(1) || fs_watcher_->nextEvent() ||
            signals_.async_wait(asio::use_awaitable));

        auto action = utils::visit(
            event,
            [&](std::string event) { return controller_->onKeyEvent(event[0]); },
            [&](notify::Event event) { return controller_->onFilesystemEvent(event); },
            [&](int signal) { return handleSignal(signal); });

        if (action == Quit) {
            co_return true;
        }

        if (action == Redraw) {
            co_return false;
        }
    }
}

asio::awaitable<void> Application::cleanup() {
    auto cleanup = core::trace::getGlobalPool().getTracer("cleanup");

    core::io::OutBuffer buffer;
    {
        auto _ = cleanup.addChild("term_clear");
        terminal_->clear(buffer);
        image_display_->unloadAll(buffer);
        co_await console_->write(buffer.view());
    }

    if (config_.system.enable_filesystem_watching) {
        auto _ = cleanup.addChild("notify_stop");
        fs_watcher_->stop();
    }

    log::debug(
        this, "total stack tracer allocations: {}", core::trace::getGlobalPool().numAllocations());

    cleanup.finalize();
    core::trace::logStackTrace<std::chrono::microseconds>(cleanup.getInfo());
}

}  // namespace app
