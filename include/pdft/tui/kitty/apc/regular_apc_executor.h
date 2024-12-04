#pragma once

#include "pdft/tui/kitty/apc/apc_executor_base.h"

namespace pdft::tui {

class RegularAPCExecutor : public APCExecutorBase {
 public:
    using APCExecutorBase::APCExecutorBase;

 private:
    void writePrefix(io::OutBuffer& buffer) const override;
    void writeSuffix(io::OutBuffer& buffer) const override;
};

}  // namespace pdft::tui
