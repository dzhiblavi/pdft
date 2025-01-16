#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/loader/loader.h"
#include "pdft/app/view/view.h"

#include "platform/event/event_emitter.h"
#include "platform/event/slot.h"
#include "platform/module/module.h"

#include "pdft/tui/renderer.h"

#include "platform/core/rc.h"

#include <asio/experimental/channel.hpp>

namespace pdft::app {

class Application : public platform::rt::Module {
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
        platform::Arc<Loader> loader,
        platform::Arc<platform::rt::EventEmitter<notify::Event>> fs_emitter,
        platform::Arc<platform::rt::EventEmitter<char>> key_emitter,
        platform::Arc<platform::rt::EventEmitter<int>> sig_emitter,
        platform::Arc<Controller> controller,
        platform::Arc<View> view,
        platform::Arc<tui::Renderer> renderer);

    platform::Async<platform::Status> run(Context& ctx);

 private:
    using Event = std::variant<
        platform::rt::Event<notify::Event>,
        platform::rt::Event<char>,
        platform::rt::Event<int>>;

    void subscribe();
    platform::Async<void> handleAction(ControllerAction action);
    platform::Async<platform::Status> renderOne();
    platform::Async<platform::Status> runApplicationLoop();

    void queueEvent(Event event);
    void onSlotEvent(const platform::rt::Event<notify::Event>& fs_event);
    void onSlotEvent(const platform::rt::Event<char>& key_event);
    void onSlotEvent(const platform::rt::Event<int>& sig_event);
    ControllerAction onEvent(const platform::rt::Event<notify::Event>& fs_event);
    ControllerAction onEvent(const platform::rt::Event<char>& key_event);
    ControllerAction onEvent(const platform::rt::Event<int>& sig_event);
    bool isEventStale(platform::SteadyTimePointType timestamp);

    platform::Async<platform::Status> shutdown();
    void stop();

    Config config_;
    asio::io_context& io_;

    platform::Arc<Loader> loader_;
    platform::Arc<View> view_;
    platform::Arc<Controller> controller_;
    platform::Arc<tui::Renderer> renderer_;

    platform::Arc<platform::rt::EventEmitter<notify::Event>> fs_emitter_;
    platform::Arc<platform::rt::EventEmitter<char>> key_emitter_;
    platform::Arc<platform::rt::EventEmitter<int>> sig_emitter_;

    platform::rt::Slot<Application, notify::Event> fs_slot_{this, &Application::onSlotEvent};
    platform::rt::Slot<Application, char> key_slot_{this, &Application::onSlotEvent};
    platform::rt::Slot<Application, int> sig_slot_{this, &Application::onSlotEvent};

    asio::experimental::channel<void(asio::error_code, Event)> events_chan_;
};

}  // namespace pdft::app
