#include "pdft/core/apc/regular/apc_executor.h"
#include "pdft/core/assert.h"

namespace pdft::core::apc::regular {

void APCExecutor::execute(APC apc, io::OutBuffer& out) {
    const size_t MaxPayloadSize = 4096;
    PDFT_VERIFY(apc.payload_b64.size() <= MaxPayloadSize);

    out << '\033' << apc.command;

    for (size_t i = 0; i < apc.control_data.size(); ++i) {
        auto&& [key, value] = apc.control_data[i];
        out << key << "=" << value;

        if (i + 1 < apc.control_data.size()) {
            out << ',';
        }
    }

    out << ';' << apc.payload_b64 << "\033\\";
}

}  // namespace pdft::core::apc::regular
