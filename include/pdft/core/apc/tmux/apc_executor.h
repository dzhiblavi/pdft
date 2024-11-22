#pragma once

#include "pdft/core/apc/apc_executor_base.h"

namespace pdft::core::apc::tmux {

class APCExecutor : public apc::APCExecutorBase {
 public:
    using APCExecutorBase::APCExecutorBase;

 private:
    void writePrefix(io::OutBuffer& buffer) const override;
    void writeSuffix(io::OutBuffer& buffer) const override;
};

}  // namespace pdft::core::apc::tmux
