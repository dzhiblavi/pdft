#include "pdft/app/app.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/log.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/runtime/module/register.h"

#include "pdft/utils/visit.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::app {

namespace {

ControllerAction combineActions(ControllerAction a, ControllerAction b) {
    using enum ControllerAction;

    if (a == Quit || b == Quit) {
        return Quit;
    }

    if (a == Redraw || b == Redraw) {
        return Redraw;
    }

    return None;
}

}  // namespace

Application::Application(
    Config config,
    asio::io_context& io,
    Arc<Loader> loader,
    Arc<rt::EventEmitter<notify::Event>> fs_emitter,
    Arc<rt::EventEmitter<char>> key_emitter,
    Arc<rt::EventEmitter<int>> sig_emitter,
    Arc<Controller> controller,
    Arc<View> view,
    Arc<tui::Renderer> renderer)
    : config_(config)
    , io_(io)
    , loader_(std::move(loader))
    , view_(std::move(view))
    , controller_(std::move(controller))
    , renderer_(std::move(renderer))
    , fs_emitter_(std::move(fs_emitter))
    , key_emitter_(std::move(key_emitter))
    , sig_emitter_(std::move(sig_emitter))
    , events_chan_(io.get_executor(), config_.event_max_buffer_size) {}

Async<Status> Application::run(Context& ctx) {
    if (auto res = ctx.push("load") | loader_->load(); !res) {
        co_return result::err(res.error());
    }

    // XXX Handle errors everywhere in this class
    ctx.push("init") | co_await renderer_->initScreen(ctx);

    subscribe();
    co_await runApplicationLoop();
    co_await shutdown();
    co_return unit;
}

Async<Status> Application::runApplicationLoop() {
    auto on_event = [&](const auto& event) {
        return utils::visit(event, [&](const auto& ev) { return onEvent(ev); });
    };

    co_await renderOne();

    for (;;) {
        auto [errc, event] = co_await events_chan_.async_receive(AsioCoroAsTuple);

        if (errc) {
            log::err(this, "failed to receive an event from the channel: {}", errc);
            continue;
        }

        auto action = on_event(event);

        while (events_chan_.ready()) {
            events_chan_.try_receive([&](asio::error_code errc, Event event) {
                if (errc) {
                    log::err(this, "try_receive failed: {}", errc);
                } else {
                    action = combineActions(action, on_event(event));
                }
            });
        }

        switch (action) {
            using enum ControllerAction;

            case None:
                break;

            case Quit:
                co_return unit;

            case Redraw:
                co_await renderOne();
                break;
        }
    }
}

void Application::subscribe() {
    fs_emitter_->subscribe(&fs_slot_);
    key_emitter_->subscribe(&key_slot_);
    sig_emitter_->subscribe(&sig_slot_);
}

Async<Status> Application::shutdown() {
    auto tracer = trace::makeTracer("shutdown");
    Context ctx(tracer);

    stop();
    co_await renderer_->restoreScreen(ctx);
    co_return loader_->save();
}

void Application::stop() {
    fs_emitter_->stop();
    key_emitter_->stop();
    sig_emitter_->stop();
}

void Application::queueEvent(Event event) {
    auto accept = [this](Event ev) -> Async<void> {
        auto [errc] = co_await events_chan_.async_send({}, ev, AsioCoroAsTuple);

        if (errc) {
            log::err(this, "failed to push an event to the channel: {}", errc);
        }
    };

    asio::co_spawn(io_, accept(std::move(event)), asio::detached);
}

void Application::onSlotEvent(const rt::Event<notify::Event>& fs_event) {
    queueEvent(Event{fs_event});
}

void Application::onSlotEvent(const rt::Event<char>& key_event) {
    queueEvent(Event{key_event});
}

void Application::onSlotEvent(const rt::Event<int>& sig_event) {
    queueEvent(Event{sig_event});
}

bool Application::isEventStale(SteadyTimePointType timestamp) {
    auto elapsed = SteadyClockType::now() - timestamp;
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    if (elapsed_ms > config_.event_timeout_ms) {
        log::warn(this, "event is stale: {}ms elapsed", elapsed_ms);
        return true;
    }

    return false;
}

ControllerAction Application::onEvent(const rt::Event<notify::Event>& fs_event) {
    return controller_->onFilesystemEvent(fs_event.value).value_or(ControllerAction::None);
}

ControllerAction Application::onEvent(const rt::Event<char>& key_event) {
    // Check staleness for keyboard events only
    if (isEventStale(key_event.timestamp)) {
        return ControllerAction::None;
    }

    return controller_->onKeyEvent(key_event.value).value_or(ControllerAction::None);
}

ControllerAction Application::onEvent(const rt::Event<int>& sig_event) {
    return controller_->onSignal(sig_event.value).value_or(ControllerAction::None);
}

Async<Status> Application::renderOne() {
    auto tracer = trace::makeTracer("render_one");
    Context ctx(tracer);

    auto dom = view_->getLayout(ctx);
    auto screen = renderer_->newFrame(ctx, std::move(dom));
    co_await view_->render(ctx, screen);
    co_await renderer_->renderFrame(ctx);
    trace::logStackTrace(std::move(tracer));

    co_return unit;
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::Application);
