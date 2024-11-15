#pragma once

#include "pdft/core/terminal/terminal.h"

namespace pdft::core::terminal::ansi {

class Terminal : public terminal::Terminal {
 public:
    void clear(OutBuffer& buffer) override;
    void moveCursor(CursorPosition position, OutBuffer& buffer) override;
};

}  // namespace pdft::core::terminal::ansi
