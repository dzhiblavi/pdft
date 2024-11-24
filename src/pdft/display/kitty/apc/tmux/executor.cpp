#include "pdft/display/kitty/apc/tmux/executor.h"

namespace pdft::display::kitty::apc::tmux {

void Executor::writePrefix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\033Ptmux;\033\033");
}

void Executor::writeSuffix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\a\033\\");
}

}  // namespace pdft::display::kitty::apc::tmux
