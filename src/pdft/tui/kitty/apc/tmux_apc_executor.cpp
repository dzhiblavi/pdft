#include "pdft/tui/kitty/apc/tmux_apc_executor.h"

#include "platform/module/register.h"

namespace pdft::tui {

void TmuxAPCExecutor::writePrefix(io::OutBuffer& buffer) const {
    io::write(buffer, "\033Ptmux;\033\033");
}

void TmuxAPCExecutor::writeSuffix(io::OutBuffer& buffer) const {
    io::write(buffer, "\a\033\\");
}

}  // namespace pdft::tui

PLATFORM_REGISTER_MODULE(pdft::tui::TmuxAPCExecutor);
