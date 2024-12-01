#pragma once

#include "pdft/tui/bufpair.h"
#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui {

struct Screen {
    terminal::Dimensions size;
    BufPair buffers;
};

}  // namespace pdft::tui
