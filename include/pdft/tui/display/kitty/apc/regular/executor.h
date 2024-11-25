#pragma once

#include "pdft/tui/display/kitty/apc/executor_base.h"

namespace pdft::tui::display::kitty::apc::regular {

class Executor : public apc::ExecutorBase {
 public:
    using ExecutorBase::ExecutorBase;

 private:
    void writePrefix(core::io::OutBuffer& buffer) const override;
    void writeSuffix(core::io::OutBuffer& buffer) const override;
};

}  // namespace pdft::tui::display::kitty::apc::regular
