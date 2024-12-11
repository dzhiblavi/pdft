#pragma once

#include "pdft/app/state/ui_state.h"
#include "pdft/notify/event.h"

#include "pdft/core/result/type/result.h"

namespace pdft::app {

enum class ControllerAction {
    None,
    Quit,
    Redraw,
};

class Controller {
 public:
    virtual ~Controller() = default;

    virtual Result<ControllerAction> onKeyEvent(
        char key, UIState& ui, DocumentStorage& documents) = 0;

    virtual Result<ControllerAction> onFilesystemEvent(
        const notify::Event& event, UIState& ui, DocumentStorage& documents) = 0;

    virtual Result<ControllerAction> onSignal(
        int signal, UIState& ui, DocumentStorage& documents) = 0;
};

}  // namespace pdft::app
