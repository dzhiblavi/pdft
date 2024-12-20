#include "pdft/tui/kitty/apc/regular_apc_executor.h"
#include "pdft/runtime/module/register.h"

namespace pdft::tui {

void RegularAPCExecutor::writePrefix(io::OutBuffer& buffer) const {
    io::write(buffer, '\033');
}

void RegularAPCExecutor::writeSuffix(io::OutBuffer& buffer) const {
    io::write(buffer, "\033\\");
}

}  // namespace pdft::tui

PDFT_REGISTER_MODULE(pdft::tui::RegularAPCExecutor);
