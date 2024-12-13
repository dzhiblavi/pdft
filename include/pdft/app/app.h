#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/event/event_listener.h"
#include "pdft/app/loader/loader.h"
#include "pdft/app/state/document_storage.h"
#include "pdft/app/view/view.h"

#include "pdft/tui/renderer.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"
#include "pdft/core/rc.h"

#include <asio/signal_set.hpp>

namespace pdft::app {

class Application : public Module {
 public:
    Application(
        inject<Loader, "loader"> loader,
        inject<EventListener, "listener"> listener,
        inject<Controller, "controller"> controller,
        inject<View, "view"> view,
        inject<tui::Renderer, "renderer"> renderer);

    AsyncStatus run(Context& ctx);

 private:
    AsyncStatus runEventLoop();
    Async<bool> waitForNextRender();

    Arc<DocumentStorage> documents_;
    Arc<Loader> loader_;
    Arc<EventListener> listener_;
    Arc<View> view_;
    Arc<Controller> controller_;
    Arc<tui::Renderer> renderer_;
};

}  // namespace pdft::app
