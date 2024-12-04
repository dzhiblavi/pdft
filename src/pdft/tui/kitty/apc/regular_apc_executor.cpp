#include "pdft/tui/kitty/apc/regular_apc_executor.h"

namespace pdft::tui {

void RegularAPCExecutor::writePrefix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, '\033');
}

void RegularAPCExecutor::writeSuffix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\033\\");
}

}  // namespace pdft::tui
