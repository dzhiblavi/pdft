#pragma once

#include "pdft/core/context.h"
#include "pdft/core/io/buffer.h"
#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui {

struct Context : core::Context {
    terminal::Terminal& terminal;
    terminal::Dimensions screen_size;

    // Buffer for control data
    core::io::OutBuffer& control_buffer;

    // Buffer for displayed data
    core::io::OutBuffer& display_buffer;

    Context(
        terminal::Terminal& term,
        core::io::OutBuffer& control_buffer,
        core::io::OutBuffer& display_buffer)
        : terminal(term)
        , screen_size(term.getDimensions())
        , control_buffer(control_buffer)
        , display_buffer(display_buffer) {}
};

}  // namespace pdft::tui
