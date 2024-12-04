#pragma once

#include "config.h"

#include "pdft/tui/display.h"
#include "pdft/tui/terminal.h"

namespace app {

pdft::Arc<pdft::tui::Display> resolveDisplay(
    const DisplayConfig& config, pdft::Arc<const pdft::tui::Terminal> terminal);

}  // namespace app
