#pragma once

#include "config.h"

#include "pdft/tui/display/display.h"
#include "pdft/tui/terminal/terminal.h"

namespace app {

pdft::Arc<pdft::tui::display::Display> resolveDisplay(
    const DisplayConfig& config, pdft::Arc<const pdft::tui::terminal::Terminal> terminal);

}  // namespace app
