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
    Config config,
    inject<Loader, "loader"> loader,
    inject<EventListener, "listener"> listener,
    inject<Controller, "controller"> controller,
    inject<View, "view"> view,
    inject<tui::Renderer, "renderer"> renderer)
    : config_(config)
    , loader_(std::move(loader))
    , listener_(std::move(listener))
    , view_(std::move(view))
    , controller_(std::move(controller))
    , renderer_(std::move(renderer)) {}

AsyncStatus Application::run(Context& ctx) {
    if (auto res = ctx.push("load") | loader_->load(); !res) {
        co_return result::err(res.error());
    }

    // XXX Handle errors everywhere in this class
    ctx.push("init") | co_await renderer_->initScreen(ctx);
    ctx.push("run") | co_await runEventLoop();
    listener_->stop();
    ctx.push("restore") | co_await renderer_->restoreScreen(ctx);
    co_return ctx.push("save") | loader_->save();
}

AsyncStatus Application::runEventLoop() {
    for (;;) {
        auto tracer = trace::makeTracer("controller");
        Context ctx(tracer);

        auto dom = view_->getLayout(ctx);
        auto screen = renderer_->newFrame(ctx, std::move(dom));
        co_await view_->render(ctx, screen);
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

        auto&& [ts, ev] = *event;
        const auto elapsed = ClockType::now() - ts;

        if (elapsed > std::chrono::milliseconds(config_.event_timeout_ms)) {
            log::warn(this, "ignoring stale event");
            continue;
        }

        log::info(
            this,
            "event handle latency: {}ms",
            std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());

        auto action =
            tracer.addChild("handle_event") |
            utils::visit(
                ev,
                [&](char c) { return controller_->onKeyEvent(c); },
                [&](const notify::Event& r) { return controller_->onFilesystemEvent(r); },
                [&](int signal) { return controller_->onSignal(signal); });

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
