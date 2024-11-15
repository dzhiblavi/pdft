#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/core/terminal/apc.h"

namespace pdft::core::terminal {

class APCExecutor {
 public:
    virtual ~APCExecutor() = default;

    virtual void execute(ApplciationProgrammingCommandView apc, OutBuffer& buffer) = 0;
};

}  // namespace pdft::core::terminal
