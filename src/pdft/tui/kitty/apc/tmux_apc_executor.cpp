#include "pdft/tui/kitty/apc/tmux_apc_executor.h"

namespace pdft::tui {

void TmuxAPCExecutor::writePrefix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\033Ptmux;\033\033");
}

void TmuxAPCExecutor::writeSuffix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\a\033\\");
}

}  // namespace pdft::tui
