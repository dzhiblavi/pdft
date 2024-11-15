#include "pdft/core/terminal/tmux/apc_executor.h"

#include <cassert>

namespace pdft::core::terminal::tmux {

void APCExecutor::execute(ApplciationProgrammingCommandView apc, OutBuffer& out) {
    const size_t MaxPayloadSize = 4096;
    assert(apc.payload_b64.size() <= MaxPayloadSize);

    out << "\033Ptmux;\033\033" << apc.command;

    for (size_t i = 0; i < apc.control_data.size(); ++i) {
        auto&& [key, value] = apc.control_data[i];
        out << key << "=" << value;

        if (i + 1 < apc.control_data.size()) {
            out << ',';
        }
    }

    out << ';' << apc.payload_b64 << "\a\033\\";
}

}  // namespace pdft::core::terminal::tmux
