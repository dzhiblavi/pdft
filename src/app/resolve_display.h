#pragma once

#include "pdft/core/io/console.h"
#include "pdft/tui/display/display.h"

#include <memory>

namespace app {

std::shared_ptr<pdft::tui::display::Display> resolveDisplay(
    std::shared_ptr<pdft::core::io::Console> console);

}  // namespace app
