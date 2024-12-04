#pragma once

#include "pdft/tui/terminal.h"
#include "pdft/tui/bufpair.h"

namespace pdft::tui {

struct Screen {
    TerminalDimensions size;
    BufPair buffers;
};

}  // namespace pdft::tui
