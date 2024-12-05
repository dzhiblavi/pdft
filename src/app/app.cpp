#include "app.h"
#include "logging.h"
#include "palettes.h"
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

#include "pdft/notify/fsw_notify.h"
#include "pdft/tui/ansi_terminal.h"
#include "pdft/tui/render.h"

#include "pdft/utils/build_info.h"
#include "pdft/utils/random.h"
#include "pdft/utils/visit.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/io/posix_console.h"
#include "pdft/core/io/posix_termctl.h"
#include "pdft/core/log.h"
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

using pdft::app::AppController;
using pdft::app::AppDocumentView;
using pdft::app::AppPageRenderer;
using pdft::app::AppView;
using pdft::app::ControllerAction;

namespace {

Result<ControllerAction> handleSignal(int signal) {
    switch (signal) {
        using enum ControllerAction;

        case SIGWINCH:
            log::info("received SIGWINCH: terminal resized, redrawing");
            return Redraw;

        case SIGINT:
            log::info("received SIGINT: exiting");
            return Quit;

        case SIGSEGV:
            return panic("received SIGSEGV");

        default:
            return panic("received unknown signal ({})", signal);
    }
}

}  // namespace

Application::Application(asio::io_context& context, Config config, pdft::app::StateMode state_mode)
    : config_(config)
    , context_(context)
    , state_mode_(state_mode)
    , signals_(context, SIGWINCH, SIGINT, SIGSEGV) {
    setup();
}

void Application::setup() {
    auto&& mupdf_context = media::MupdfContext::thisThreadInstance();
    ::app::setupMupdfLogging(mupdf_context);
    mupdf_context.registerDocumentHandlers();

    term_control_ = std::make_shared<io::PosixTermControl>();
    terminal_ = std::make_shared<tui::AnsiTerminal>();
    console_ =
        std::make_shared<io::PosixConsole>(context_, ::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));
    image_display_ = resolveDisplay(config_.display, terminal_);
    doc_loader_ = std::make_shared<pdft::media::MupdfLoader>(mupdf_context);

    auto persister = std::make_shared<pdft::app::AppPersister>(doc_loader_);
    loader_ = std::make_shared<pdft::app::AppLoader>(persister, doc_loader_);

    documents_ = std::make_shared<pdft::app::DocumentStorage>();
    palettes_ = std::make_shared<pdft::app::PaletteStorage>();
    palettes_->add("skim", getSkimPalette());

    ui_.scaling_coeff = config_.display.scaling_coeff;
    ui_.invert_colors = config_.display.invert_colors;

    auto renderer = std::make_shared<AppPageRenderer>(
        utils::random<uint32_t>(), documents_, palettes_, image_display_);
    auto document_view = std::make_shared<AppDocumentView>(std::move(renderer));
    view_ = std::make_shared<AppView>(documents_, std::move(document_view));
    controller_ = std::make_shared<AppController>(config_.controller, doc_loader_);

    if (config_.system.enable_filesystem_watching) {
        fs_watcher_ = std::make_shared<pdft::notify::FswNotify>(context_.get_executor());
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
    term_control_->ioControl(io::ControlType::Input, {.canonical = false, .echo = false});

    // Clear the screen
    io::OutBuffer init_buffer;
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
        return;
    }

    for (auto&& doc : ui_.states) {
        auto document = documents_->getDocument(doc.document_id) | result::unwrap;
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

    if (pdft::app::shouldSave(state_mode_)) {
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

pdft::AsyncResult<int> Application::awaitSignal() {
    auto [ec, signal] = co_await signals_.async_wait(asio_coro_as_tuple);

    if (ec) {
        co_return result::err(ec);
    }

    co_return result::ok(signal);
}

asio::awaitable<bool> Application::waitForNextRender() {
    for (;;) {
        using enum ControllerAction;

        // XXX Debouncing?
        auto event = co_await (console_->read(1) || fs_watcher_->nextEvent() || awaitSignal());

        // XXX Handle errors!
        auto action = utils::visitUnpack(
            event,
            [&](Result<std::string> r) {
                char c = (r | result::unwrap)[0];
                return controller_->onKeyEvent(c, ui_, *documents_);
            },
            [&](Result<notify::Event> r) {
                auto&& event = r | result::unwrap;
                return controller_->onFilesystemEvent(event, ui_, *documents_);
            },
            [&](Result<int> r) {
                int signal = r | result::unwrap;
                return handleSignal(signal);
            });

        if (action.has_value()) {
            if (action == Quit) {
                co_return true;
            }
            if (action == Redraw) {
                co_return false;
            }
        } else {
            log::err("error in controller loop: {}", action.error());
        }
    }
}

asio::awaitable<void> Application::cleanup() {
    auto tracer = trace::getGlobalPool().getTracer("cleanup");
    Context ctx;
    ctx.setTracer(tracer);

    ctx.push("term_clear") | co_await [&] {
        io::OutBuffer buffer;
        terminal_->showCursor(buffer);
        terminal_->clear(buffer);
        image_display_->unloadAll(ctx, buffer);
        return console_->write(buffer.view());
    }();

    if (config_.system.enable_filesystem_watching) {
        auto _ = ctx.push("notify_stop");
        fs_watcher_->stop();
    }

    log::debug(this, "total stack tracer allocations: {}", trace::getGlobalPool().numAllocations());

    tracer.finalize();
    trace::logStackTrace(tracer.getInfo());
}

}  // namespace app
