#include "pdft/app/app.h"

#include "pdft/tui/render.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/log.h"
#include "pdft/core/module/register.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/utils/visit.h"

#include <asio/experimental/awaitable_operators.hpp>
#include <asio/io_context.hpp>

#include <ftxui/screen/screen.hpp>
#include <magic_enum.hpp>

namespace pdft::app {

using namespace asio::experimental::awaitable_operators;

Application::Application(
    inject<AppLoader, "loader"> loader,
    inject<EventListener, "listener"> listener,
    inject<io::ConsoleWriter, "writer"> writer,
    inject<io::TermControl, "term_ctrl"> term_control,
    inject<tui::Terminal, "terminal"> terminal,
    inject<tui::Display, "display"> display,
    inject<DocumentStorage, "docs"> documents,
    inject<Controller, "controller"> controller,
    inject<View, "view"> view)
    : loader_(std::move(loader))
    , listener_(std::move(listener))
    , term_control_(std::move(term_control))
    , console_writer_(std::move(writer))
    , terminal_(std::move(terminal))
    , image_display_(std::move(display))
    , documents_(std::move(documents))
    , view_(std::move(view))
    , controller_(std::move(controller)) {}

AsyncStatus Application::run() {
    if (auto res = loader_->load(ui_, *documents_); !res) {
        co_return result::err(res.error());
    }

    co_await prepareTerminal();
    co_await runEventLoop();
    co_await cleanup();

    co_return loader_->save(ui_, *documents_);
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

asio::awaitable<bool> Application::waitForNextRender() {
    auto tracer = trace::getGlobalPool().getTracer("controller");

    for (;;) {
        using enum ControllerAction;

        // XXX Debouncing?
        auto event = tracer.addChild("wait_input") | co_await listener_->nextEvent();

        if (!event) {
            log::err(this, "error event received: {}", event.error());
            continue;
        }

        auto action =
            tracer.addChild("handle_event") |
            utils::visit(
                *event,
                [&](char c) { return controller_->onKeyEvent(c, ui_, *documents_); },
                [&](const notify::Event& r) {
                    return controller_->onFilesystemEvent(r, ui_, *documents_);
                },
                [&](int signal) { return controller_->onSignal(signal, ui_, *documents_); });

        if (action.has_value()) {
            if (action == None) {
                continue;
            }

            trace::logStackTrace(std::move(tracer));
            co_return action == Quit;
        } else {
            log::err(this, "error fron controller: {}", action.error());
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

    log::debug(this, "total stack tracer allocations: {}", trace::getGlobalPool().numAllocations());
    trace::logStackTrace(std::move(tracer));
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::Application);
