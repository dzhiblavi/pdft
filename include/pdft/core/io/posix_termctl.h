#pragma once

#include "pdft/core/io/termctl.h"
#include "pdft/core/module/module.h"

namespace pdft::io {

class PosixTermControl : public TermControl, public Module {
 public:
    void ioControl(ControlType type, const IoControl& control) override;
};

}  // namespace pdft::io
