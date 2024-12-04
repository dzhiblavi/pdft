#pragma once

#include "pdft/app/state/ui_state.h"
#include "pdft/notify/event.h"

#include "pdft/core/result/type/result.h"

namespace pdft::app::controller {

enum class ControllerError {
    UnkownFilePath,
};

enum class Action {
    None,
    Quit,
    Redraw,
};

class Controller {
 public:
    virtual ~Controller() = default;

    virtual Result<Action> onKeyEvent(char key, UIState& ui, DocumentStorage& documents) = 0;

    virtual Result<Action> onFilesystemEvent(
        const notify::Event& event, UIState& ui, DocumentStorage& documents) = 0;
};

}  // namespace pdft::app::controller
