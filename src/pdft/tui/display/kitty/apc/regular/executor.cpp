#include "pdft/tui/display/kitty/apc/regular/executor.h"

namespace pdft::tui::display::kitty::apc::regular {

void Executor::writePrefix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, '\033');
}

void Executor::writeSuffix(core::io::OutBuffer& buffer) const {
    core::io::write(buffer, "\033\\");
}

}  // namespace pdft::tui::display::kitty::apc::regular
