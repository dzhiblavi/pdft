#include "pdft/app/app.h"

#include "pdft/tui/render.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/log.h"
#include "pdft/core/module/register.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/utils/get_home_dir.h"
#include "pdft/utils/visit.h"

#include <asio/experimental/awaitable_operators.hpp>
#include <asio/io_context.hpp>

#include <ftxui/screen/screen.hpp>
#include <magic_enum.hpp>

namespace pdft::app {

using namespace asio::experimental::awaitable_operators;

Application::Application(
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
    inject<pdft::app::View, "view"> view)
    : config_(config)
    , signals_(context, SIGWINCH, SIGINT, SIGSEGV)
    , fs_watcher_(std::move(fswatcher))
    , term_control_(std::move(term_control))
    , console_writer_(std::move(writer))
    , console_reader_(std::move(reader))
    , terminal_(std::move(terminal))
    , image_display_(std::move(display))
    , loader_(std::move(loader))
    , documents_(std::move(documents))
    , view_(std::move(view))
    , controller_(std::move(controller)) {}

Result<ControllerAction> Application::handleSignal(int signal) {
    switch (signal) {
        using enum ControllerAction;

        case SIGWINCH:
            log::info(this, "received SIGWINCH: terminal resized, redrawing");
            return Redraw;

        case SIGINT:
            log::info(this, "received SIGINT: exiting");
            return Quit;

        case SIGSEGV:
            return panic("received SIGSEGV");

        default:
            return panic("received unknown signal ({})", signal);
    }
}

asio::awaitable<void> Application::run(const std::vector<std::string>& paths) {
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
    co_await console_writer_->write(init_buffer);
}

void Application::load(const std::vector<std::string>& paths) {
    if (!config_.state_path.has_value()) {
        log::info(this, "not restoring the state");
        return;
    }

    auto state_path = std::filesystem::absolute(utils::expandHome(*config_.state_path));

    auto err = loader_->load(paths, ui_, *documents_, state_path);
    if (!err.has_value()) {
        log::err(this, "failed to load state_path={}, error={}", *config_.state_path, err.error());
        return;
    }

    for (auto&& doc : ui_.states) {
        auto document = documents_->getDocument(doc.document_id) | result::unwrap;
        const auto& path = document->getPath();

        log::info(this, "adding path {} to filesystem watching", path);
        fs_watcher_->add(path);
    }
}

void Application::saveState() {
    if (!config_.state_path.has_value()) {
        log::info(this, "not saving the state");
        return;
    }

    auto state_path = std::filesystem::absolute(utils::expandHome(*config_.state_path));
    loader_->save(ui_, *documents_, state_path);
}

asio::awaitable<void> Application::runEventLoop() {
    pdft::tui::Renderer renderer(terminal_, console_writer_);

    for (;;) {
        co_await renderer.renderOne(
            [this](auto& ctx) { return view_->getLayout(ctx, ui_); },
            [this](auto& ctx, auto& screen) -> Async<Unit> {
                image_display_->clear(ctx, screen.bufpair().control_buffer);
                co_await view_->render(ctx, ui_, screen);
                co_return unit;
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
    auto tracer = trace::getGlobalPool().getTracer("controller");

    for (;;) {
        using enum ControllerAction;

        // XXX Debouncing?
        auto event =
            tracer.addChild("wait_input") |
            co_await (console_reader_->read(1) || fs_watcher_->nextEvent() || awaitSignal());

        // XXX Handle errors!
        auto action =
            tracer.addChild("handle_event") |
            utils::visitUnpack(
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
            if (action == None) {
                continue;
            }

            trace::logStackTrace(std::move(tracer));
            co_return action == Quit;
        } else {
            log::err(this, "error in controller loop: {}", action.error());
        }
    }
}

asio::awaitable<void> Application::cleanup() {
    auto tracer = trace::getGlobalPool().getTracer("cleanup");
    Context ctx;
    ctx.setTracer(tracer);

    ctx.push("term:clear") | co_await [&]() -> AsyncStatus {
        io::OutBuffer buffer;
        terminal_->showCursor(buffer);
        terminal_->clear(buffer);
        image_display_->unloadAll(ctx, buffer);
        co_return co_await console_writer_->write(buffer.view());
    }();

    {
        auto _ = ctx.push("fsw:stop");
        fs_watcher_->stop();
    }

    log::debug(this, "total stack tracer allocations: {}", trace::getGlobalPool().numAllocations());
    trace::logStackTrace(std::move(tracer));
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::Application);
