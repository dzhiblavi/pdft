#pragma once

#include "pdft/io/termctl.h"

#include "pdft/runtime/module/module.h"

namespace pdft::io {

class PosixTermControl : public TermControl, public rt::Module {
 public:
    void ioControl(ControlType type, const IoControl& control) override;
};

}  // namespace pdft::io
