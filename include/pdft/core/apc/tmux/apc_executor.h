#pragma once

#include "pdft/core/apc/apc_executor_base.h"

namespace pdft::core::apc::tmux {

class APCExecutor : public apc::APCExecutorBase {
 public:
    using APCExecutorBase::APCExecutorBase;

    void execute(APC apc, io::OutBuffer& buffer) override;
};

}  // namespace pdft::core::apc::tmux
