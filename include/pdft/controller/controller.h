#pragma once

#include "pdft/core/notify/notify.h"

namespace pdft::controller {

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

}  // namespace pdft::controller
