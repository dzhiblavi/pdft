#pragma once

#include "pdft/tui/bufpair.h"
#include "pdft/tui/terminal.h"

namespace pdft::tui {

class Screen {
 public:
    Screen(TerminalDimensions dims, io::OutBuffer& control, io::OutBuffer& display)
        : dims_(dims)
        , bufpair_(control, display) {}

    const TerminalDimensions& dimensions() const {
        return dims_;
    }

    BufPair& bufpair() {
        return bufpair_;
    }

 private:
    TerminalDimensions dims_;
    BufPair bufpair_;
};

}  // namespace pdft::tui
