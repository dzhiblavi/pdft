#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui {

struct Context {
    terminal::Terminal& terminal;
    terminal::Dimensions screen_size;
    core::io::OutBuffer& buffer;
};

}  // namespace pdft::tui
