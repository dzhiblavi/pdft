#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui {

struct ContextRef {
    terminal::Terminal& terminal;
    core::io::OutBuffer& buffer;
};

}  // namespace pdft::tui
