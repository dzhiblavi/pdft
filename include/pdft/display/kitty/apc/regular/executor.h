#pragma once

#include "pdft/display/kitty/apc/executor_base.h"

namespace pdft::display::kitty::apc::regular {

class Executor : public apc::ExecutorBase {
 public:
    using ExecutorBase::ExecutorBase;

 private:
    void writePrefix(core::io::OutBuffer& buffer) const override;
    void writeSuffix(core::io::OutBuffer& buffer) const override;
};

}  // namespace pdft::display::kitty::apc::regular
