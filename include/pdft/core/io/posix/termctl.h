#pragma once

#include "pdft/core/io/termctl.h"

namespace pdft::core::io::posix {

class TermControl : public io::TermControl {
 public:
    void ioControl(ControlType type, const IoControl& control) override;
};

}  // namespace pdft::core::io::posix
