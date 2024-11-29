#pragma once

#include "config.h"

#include "pdft/tui/display/display.h"

#include <memory>

namespace app {

std::shared_ptr<pdft::tui::display::Display> resolveDisplay(const DisplayConfig& config);

}  // namespace app
