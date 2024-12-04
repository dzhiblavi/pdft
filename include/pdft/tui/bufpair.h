#pragma once

#include "pdft/core/io/buffer.h"

namespace pdft::tui {

struct BufPair {
    io::OutBuffer& control_buffer;
    io::OutBuffer& display_buffer;
};

}  // namespace pdft::tui
