#pragma once

#include "pdft/notify/event.h"

#include "pdft/core/result.h"

namespace pdft::app {

enum class ControllerAction {
    None,
    Quit,
    Redraw,
};

class Controller {
 public:
    virtual ~Controller() = default;

    virtual Result<ControllerAction> onKeyEvent(char key) = 0;
    virtual Result<ControllerAction> onFilesystemEvent(const notify::Event& event) = 0;
    virtual Result<ControllerAction> onSignal(int signal) = 0;
};

}  // namespace pdft::app
