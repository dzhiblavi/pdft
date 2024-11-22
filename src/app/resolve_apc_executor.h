#pragma once

#include "pdft/core/apc/apc_executor.h"
#include "pdft/core/io/console.h"

#include <memory>

namespace app {

std::shared_ptr<pdft::core::apc::APCExecutor> resolveAPCExecutor(
    std::shared_ptr<pdft::core::io::Console> console);

}  // namespace app
