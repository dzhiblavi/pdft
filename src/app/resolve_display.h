#pragma once

#include "pdft/core/terminal/apc_executor.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/view/image/display.h"

#include <memory>

namespace app {

std::shared_ptr<pdft::view::image::Display> resolveDisplay(
    std::shared_ptr<pdft::core::terminal::Terminal> terminal,
    std::shared_ptr<pdft::core::terminal::APCExecutor> executor);

}  // namespace app
