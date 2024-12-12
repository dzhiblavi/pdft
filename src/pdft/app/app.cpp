#include "pdft/app/app.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/log.h"
#include "pdft/core/module/register.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/utils/visit.h"

namespace pdft::app {

Application::Application(
    inject<DocumentStorage, "docs"> documents,
    inject<Loader, "loader"> loader,
    inject<EventListener, "listener"> listener,
    inject<Controller, "controller"> controller,
    inject<View, "view"> view,
    inject<tui::Renderer, "renderer"> renderer)
    : documents_(std::move(documents))
    , loader_(std::move(loader))
    , listener_(std::move(listener))
    , view_(std::move(view))
    , controller_(std::move(controller))
    , renderer_(std::move(renderer)) {}

AsyncStatus Application::run(Context& ctx) {
    if (auto res = ctx.push("load") | loader_->load(ui_, *documents_); !res) {
        co_return result::err(res.error());
    }

    // XXX Handle errors everywhere in this class
    ctx.push("init") | co_await renderer_->initScreen(ctx);
    ctx.push("run") | co_await runEventLoop();
    ctx.push("restore") | co_await renderer_->restoreScreen(ctx);

    co_return ctx.push("save") | loader_->save(ui_, *documents_);
}

AsyncStatus Application::runEventLoop() {
    for (;;) {
        auto tracer = trace::makeTracer("controller");
        Context ctx(tracer);

        auto dom = view_->getLayout(ctx, ui_);
        auto screen = renderer_->newFrame(ctx, std::move(dom));
        co_await view_->render(ctx, ui_, screen);
        co_await renderer_->renderFrame(ctx);

        trace::logStackTrace(std::move(tracer));

        if (co_await waitForNextRender()) {
            co_return unit;
        }
    }
}

asio::awaitable<bool> Application::waitForNextRender() {
    auto tracer = trace::makeTracer("controller");

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

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::Application);
