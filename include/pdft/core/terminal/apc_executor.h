#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/core/terminal/apc.h"

namespace pdft::core::terminal {

class APCExecutor {
 public:
    virtual ~APCExecutor() = default;

    virtual void execute(ApplciationProgrammingCommandView apc, io::OutBuffer& buffer) = 0;
};

}  // namespace pdft::core::terminal
