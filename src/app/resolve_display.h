#pragma once

#include "pdft/core/apc/apc_executor.h"
#include "pdft/core/io/console.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/display/display.h"

#include <memory>

namespace app {

std::shared_ptr<pdft::display::Display> resolveDisplay(
    std::shared_ptr<pdft::core::terminal::Terminal> terminal,
    std::shared_ptr<pdft::core::apc::APCExecutor> executor,
    std::shared_ptr<pdft::core::io::Console> console);

}  // namespace app
