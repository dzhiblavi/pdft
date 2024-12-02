#include "app.h"
#include "logging.h"
#include "resolve_display.h"
#include "version.h"

#include "pdft/app/controller/impl/controller.h"
#include "pdft/app/loader/impl/loader.h"
#include "pdft/app/persist/impl/persister.h"
#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"
#include "pdft/app/view/impl/document_view.h"
#include "pdft/app/view/impl/page_renderer.h"
#include "pdft/app/view/impl/view.h"

#include "pdft/media/mupdf/context.h"
#include "pdft/media/mupdf/loader.h"

#include "pdft/notify/fsw/notify.h"
#include "pdft/tui/render.h"
#include "pdft/tui/terminal/ansi/terminal.h"

#include "pdft/utils/build_info.h"
#include "pdft/utils/random.h"
#include "pdft/utils/visit.h"

#include "pdft/core/io/posix/console.h"
#include "pdft/core/io/posix/termctl.h"
#include "pdft/core/log/log.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include <asio/experimental/awaitable_operators.hpp>
#include <asio/io_context.hpp>

#include <ftxui/screen/screen.hpp>
#include <magic_enum.hpp>

#include <fstream>

namespace app {

using namespace pdft;
using namespace asio::experimental::awaitable_operators;

using pdft::app::controller::Action;

using pdft::app::view::impl::DocumentView;
using pdft::app::view::impl::PageRenderer;
using pdft::app::view::impl::View;

using pdft::app::controller::impl::Controller;

namespace {

Action handleSignal(int signal) {
    switch (signal) {
        using enum Action;

        case SIGWINCH:
            log::info("received SIGWINCH: terminal resized, redrawing");
            return Redraw;

        case SIGINT:
            log::info("received SIGINT: exiting");
            return Quit;

        case SIGSEGV:
            return core::panic("received SIGSEGV");

        default:
            return core::panic("received unknown signal ({})", signal);
    }
}

}  // namespace

Application::Application(
    asio::io_context& context, Config config, pdft::app::loader::StateMode state_mode)
    : context_(context)
    , config_(config)
    , state_mode_(state_mode)
    , signals_(context, SIGWINCH, SIGINT, SIGSEGV) {
    setup();
}

void Application::setup() {
    ::app::setupMupdfLogging(media::mupdf::thisThreadContext());
    media::mupdf::thisThreadContext().registerDocumentHandlers();

    term_control_ = std::make_shared<core::io::posix::TermControl>();
    terminal_ = std::make_shared<tui::terminal::ansi::Terminal>();
    console_ = std::make_shared<core::io::posix::Console>(
        context_, ::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));
    image_display_ = resolveDisplay(config_.display, terminal_);
    doc_loader_ =
        std::make_shared<pdft::media::mupdf::Loader>(pdft::media::mupdf::thisThreadContext());

    auto persister = std::make_shared<pdft::app::persist::impl::Persister>(doc_loader_);
    loader_ = std::make_shared<pdft::app::loader::impl::Loader>(persister, doc_loader_);

    documents_ = std::make_shared<pdft::app::state::DocumentStorage>();
    ui_.scaling_coeff = config_.display.scaling_coeff;
    ui_.invert_colors = config_.display.invert_colors;

    auto renderer =
        std::make_shared<PageRenderer>(utils::random<uint32_t>(), documents_, image_display_);
    auto document_view = std::make_shared<DocumentView>(std::move(renderer));
    view_ = std::make_shared<View>(documents_, std::move(document_view));
    controller_ = std::make_shared<Controller>(config_.controller, doc_loader_);

    if (config_.system.enable_filesystem_watching) {
        fs_watcher_ = std::make_shared<pdft::notify::fsw::Notify>(context_.get_executor());
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
    load(paths);
    co_await runEventLoop();
    saveState();
    co_await cleanup();
}

asio::awaitable<void> Application::prepareTerminal() {
    // Configure terminal
    term_control_->ioControl(core::io::ControlType::Input, {.canonical = false, .echo = false});

    // Clear the screen
    core::io::OutBuffer init_buffer;
    terminal_->hideCursor(init_buffer);
    terminal_->clear(init_buffer);
    co_await console_->write(init_buffer);
}

void Application::load(const std::vector<std::string>& paths) {
    if (!config_.state_path.has_value()) {
        log::info("not restoring the state");
        return;
    }

    log::info("restoring the state from {}", *config_.state_path);
    auto state_path = std::filesystem::absolute(utils::expandHome(*config_.state_path));
    std::ifstream ifs(state_path);

    auto err = loader_->load(paths, ui_, *documents_, state_mode_, ifs);
    if (!err.has_value()) {
        log::err("failed to load state_path={}, error={}", *config_.state_path, err.error());
        return;
    }

    if (!config_.system.enable_filesystem_watching) {
        log::info("filesystem watching is disabled");
    }

    for (auto&& doc : ui_.states) {
        auto maybe_doc = documents_->getDocument(doc.document_id);
        PDFT_VERIFY(maybe_doc);
        auto document = *maybe_doc;
        const auto& path = document->getPath();

        log::info("adding path {} to filesystem watching", path);
        fs_watcher_->add(path);
    }
}

void Application::saveState() {
    if (!config_.state_path.has_value()) {
        log::info("not saving the state");
        return;
    }

    if (pdft::app::loader::shouldSave(state_mode_)) {
        auto state_path = std::filesystem::absolute(utils::expandHome(*config_.state_path));
        std::ofstream ofs(state_path);
        loader_->save(ui_, *documents_, ofs);
    }
}

asio::awaitable<void> Application::runEventLoop() {
    pdft::tui::Renderer renderer(terminal_, console_);

    for (;;) {
        co_await renderer.renderOne([this](auto& ctx, auto& screen) {
            image_display_->clear(ctx, screen.buffers.control_buffer);
            return view_->render(ctx, ui_, screen);
        });

        bool should_quit = co_await waitForNextRender();

        if (should_quit) {
            break;
        }
    }
}

asio::awaitable<bool> Application::waitForNextRender() {
    for (;;) {
        using enum Action;

        // XXX debouncing?
        auto event = co_await (
            console_->read(1) || fs_watcher_->nextEvent() ||
            signals_.async_wait(asio::use_awaitable));

        auto action = utils::visit(
            event,
            [&](std::string event) { return controller_->onKeyEvent(event[0], ui_, *documents_); },
            [&](notify::Event event) {
                return controller_->onFilesystemEvent(event, ui_, *documents_);
            },
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
    auto tracer = core::trace::getGlobalPool().getTracer("cleanup");
    core::Context ctx;
    ctx.setTracer(tracer);

    {
        auto _ = ctx.push("term_clear");
        core::io::OutBuffer buffer;
        terminal_->showCursor(buffer);
        terminal_->clear(buffer);
        image_display_->unloadAll(ctx, buffer);
        co_await console_->write(buffer.view());
    }

    if (config_.system.enable_filesystem_watching) {
        auto _ = ctx.push("notify_stop");
        fs_watcher_->stop();
    }

    log::debug(
        this, "total stack tracer allocations: {}", core::trace::getGlobalPool().numAllocations());

    tracer.finalize();
    core::trace::logStackTrace(tracer.getInfo());
}

}  // namespace app
