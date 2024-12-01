#pragma once

#include "config.h"

#include "pdft/tui/display/display.h"
#include "pdft/tui/terminal/terminal.h"

#include <memory>

namespace app {

std::shared_ptr<pdft::tui::display::Display> resolveDisplay(
    const DisplayConfig& config, std::shared_ptr<const pdft::tui::terminal::Terminal> terminal);

}  // namespace app
