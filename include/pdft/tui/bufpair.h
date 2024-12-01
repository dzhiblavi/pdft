#pragma once

#include "pdft/core/io/buffer.h"

namespace pdft::tui {

struct BufPair {
    core::io::OutBuffer& control_buffer;
    core::io::OutBuffer& display_buffer;
};

}  // namespace pdft::tui
