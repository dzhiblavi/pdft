#pragma once

#include "pdft/app/state/ui_state.h"
#include "pdft/notify/event.h"

namespace pdft::app::controller {

enum class Action {
    None,
    Quit,
    Redraw,
};

class Controller {
 public:
    virtual ~Controller() = default;

    virtual Action onKeyEvent(char key, state::UIState& ui, state::DocumentStorage& documents) = 0;

    virtual Action onFilesystemEvent(
        const notify::Event& event, state::UIState& ui, state::DocumentStorage& documents) = 0;
};

}  // namespace pdft::app::controller
