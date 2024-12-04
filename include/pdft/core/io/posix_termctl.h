#pragma once

#include "pdft/core/io/termctl.h"

namespace pdft::io {

class PosixTermControl : public TermControl {
 public:
    void ioControl(ControlType type, const IoControl& control) override;
};

}  // namespace pdft::io
