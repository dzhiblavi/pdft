#pragma once

#include "pdft/core/terminal/apc_executor.h"

namespace pdft::core::terminal::tmux {

class APCExecutor : public terminal::APCExecutor {
 public:
    void execute(ApplciationProgrammingCommandView apc, io::OutBuffer& buffer) override;
};

}  // namespace pdft::core::terminal::tmux
