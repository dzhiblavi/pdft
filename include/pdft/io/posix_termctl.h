#pragma once

#include "pdft/io/termctl.h"

#include "platform/module/module.h"

namespace pdft::io {

class PosixTermControl : public TermControl, public platform::rt::Module {
 public:
    void ioControl(ControlType type, const IoControl& control) override;
};

}  // namespace pdft::io
