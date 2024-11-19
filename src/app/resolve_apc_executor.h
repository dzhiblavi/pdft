#pragma once

#include "pdft/core/terminal/apc_executor.h"

#include <memory>

namespace app {

std::shared_ptr<pdft::core::terminal::APCExecutor> resolveAPCExecutor();

}  // namespace app
