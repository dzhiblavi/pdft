#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/event/event_listener.h"
#include "pdft/app/loader/loader.h"
#include "pdft/app/view/view.h"

#include "pdft/tui/renderer.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"
#include "pdft/core/rc.h"

namespace pdft::app {

class Application : public Module {
 public:
    struct Config {
        static constexpr int DefaultEventTimeoutMs = 100;

        int event_timeout_ms = DefaultEventTimeoutMs;
    };

    Application(
        Config config,
        inject<Loader, "loader"> loader,
        inject<EventListener, "listener"> listener,
        inject<Controller, "controller"> controller,
        inject<View, "view"> view,
        inject<tui::Renderer, "renderer"> renderer);

    AsyncStatus run(Context& ctx);

 private:
    AsyncStatus runEventLoop();
    Async<bool> waitForNextRender();

    Config config_;
    Arc<Loader> loader_;
    Arc<EventListener> listener_;
    Arc<View> view_;
    Arc<Controller> controller_;
    Arc<tui::Renderer> renderer_;
};

}  // namespace pdft::app
