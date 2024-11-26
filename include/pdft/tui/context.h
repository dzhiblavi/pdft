#pragma once

#include "pdft/core/context.h"
#include "pdft/core/io/buffer.h"
#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui {

struct Context : core::Context {
    terminal::Terminal& terminal;
    terminal::Dimensions screen_size;
    core::io::OutBuffer& buffer;

    Context(terminal::Terminal& term, core::io::OutBuffer& buffer)
        : terminal(term)
        , screen_size(term.getDimensions())
        , buffer(buffer) {}
};

}  // namespace pdft::tui
