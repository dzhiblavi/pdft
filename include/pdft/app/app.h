#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/loader/loader.h"
#include "pdft/app/view/view.h"

#include "pdft/event/event_emitter.h"
#include "pdft/tui/renderer.h"

#include "pdft/core/module/module.h"
#include "pdft/core/rc.h"

#include <asio/experimental/channel.hpp>

namespace pdft::app {

class Application : public Module {
 public:
    struct Config {
        static constexpr int DefaultEventTimeoutMs = 100;
        static constexpr int DefaultEventMaxBufferSize = 10;

        int event_timeout_ms = DefaultEventTimeoutMs;
        int event_max_buffer_size = DefaultEventMaxBufferSize;
    };

    Application(
        Config config,
        asio::io_context& io,
        Arc<Loader> loader,
        Arc<event::EventEmitter<notify::Event>> fs_emitter,
        Arc<event::EventEmitter<char>> key_emitter,
        Arc<event::EventEmitter<int>> sig_emitter,
        Arc<Controller> controller,
        Arc<View> view,
        Arc<tui::Renderer> renderer);

    Async<Status> run(Context& ctx);

 private:
    using Event = std::variant<event::Event<notify::Event>, event::Event<char>, event::Event<int>>;

    void subscribe();
    Async<void> handleAction(ControllerAction action);
    Async<Status> renderOne();
    Async<Status> runApplicationLoop();

    void queueEvent(Event event);
    void onSlotEvent(const event::Event<notify::Event>& fs_event);
    void onSlotEvent(const event::Event<char>& key_event);
    void onSlotEvent(const event::Event<int>& sig_event);
    ControllerAction onEvent(const event::Event<notify::Event>& fs_event);
    ControllerAction onEvent(const event::Event<char>& key_event);
    ControllerAction onEvent(const event::Event<int>& sig_event);
    bool isEventStale(SteadyTimePointType timestamp);

    Async<Status> shutdown();
    void stop();

    Config config_;
    asio::io_context& io_;

    Arc<Loader> loader_;
    Arc<View> view_;
    Arc<Controller> controller_;
    Arc<tui::Renderer> renderer_;

    event::Slot<Application, notify::Event> fs_slot_{this, &Application::onSlotEvent};
    event::Slot<Application, char> key_slot_{this, &Application::onSlotEvent};
    event::Slot<Application, int> sig_slot_{this, &Application::onSlotEvent};

    Arc<event::EventEmitter<notify::Event>> fs_emitter_;
    Arc<event::EventEmitter<char>> key_emitter_;
    Arc<event::EventEmitter<int>> sig_emitter_;

    asio::experimental::channel<void(asio::error_code, Event)> events_chan_;
};

}  // namespace pdft::app
