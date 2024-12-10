#pragma once

#include "pdft/tui/kitty/apc/apc_executor_base.h"

#include "pdft/core/module/module.h"

namespace pdft::tui {

class RegularAPCExecutor : public APCExecutorBase, public Module {
 public:
    using Config = APCExecutorConfig;
    using APCExecutorBase::APCExecutorBase;

 private:
    void writePrefix(io::OutBuffer& buffer) const override;
    void writeSuffix(io::OutBuffer& buffer) const override;
};

}  // namespace pdft::tui
