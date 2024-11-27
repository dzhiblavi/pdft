#pragma once

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

    virtual Action onKeyEvent(char key) = 0;
    virtual Action onFilesystemEvent(const notify::Event& event) = 0;
};

}  // namespace pdft::app::controller
