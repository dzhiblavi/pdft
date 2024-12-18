#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/loader/loader.h"
#include "pdft/app/view/view.h"

#include "pdft/event/event_emitter.h"
#include "pdft/tui/renderer.h"

#include "pdft/core/module/inject.h"
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
        inject<Loader, "loader"> loader,
        inject<event::EventEmitter<notify::Event>, "fs_event_emitter"> fs_emitter,
        inject<event::EventEmitter<char>, "key_event_emitter"> key_emitter,
        inject<event::EventEmitter<int>, "sig_event_emitter"> sig_emitter,
        inject<Controller, "controller"> controller,
        inject<View, "view"> view,
        inject<tui::Renderer, "renderer"> renderer);

    Async<Status> run(Context& ctx);

 private:
    void subscribe();
    Async<void> handleAction(ControllerAction action);
    Async<Status> renderOne();
    Async<Status> runApplicationLoop();

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

    Arc<event::EventEmitter<notify::Event>> fs_emitter_;
    Arc<event::EventEmitter<char>> key_emitter_;
    Arc<event::EventEmitter<int>> sig_emitter_;

    using Event = std::variant<event::Event<notify::Event>, event::Event<char>, event::Event<int>>;
    asio::experimental::channel<void(asio::error_code, Event)> events_chan_;
};

}  // namespace pdft::app
