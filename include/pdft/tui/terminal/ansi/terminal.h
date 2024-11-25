#pragma once

#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui::terminal::ansi {

class Terminal : public terminal::Terminal {
 public:
    void clear(core::io::OutBuffer& buffer) override;
    void moveCursor(CursorPosition position, core::io::OutBuffer& buffer) override;
    void patchStyle(style::Patch patch, core::io::OutBuffer& buffer) override;

 private:
    style::Style current_style_;
};

}  // namespace pdft::tui::terminal::ansi
