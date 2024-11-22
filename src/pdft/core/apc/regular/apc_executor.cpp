#include "pdft/core/apc/regular/apc_executor.h"

namespace pdft::core::apc::regular {

void APCExecutor::writePrefix(io::OutBuffer& buffer) const {
    io::write(buffer, '\033');
}

void APCExecutor::writeSuffix(io::OutBuffer& buffer) const {
    io::write(buffer, "\033\\");
}

}  // namespace pdft::core::apc::regular
