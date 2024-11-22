#include "pdft/core/apc/tmux/apc_executor.h"

namespace pdft::core::apc::tmux {

void APCExecutor::writePrefix(io::OutBuffer& buffer) const {
    io::write(buffer, "\033Ptmux;\033\033");
}

void APCExecutor::writeSuffix(io::OutBuffer& buffer) const {
    io::write(buffer, "\a\033\\");
}

}  // namespace pdft::core::apc::tmux
