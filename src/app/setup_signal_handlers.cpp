#include "setup_signal_handlers.h"

#include "pdft/core/assert.h"

#include <csignal>
#include <format>

namespace app {

[[noreturn]] void sigsegvHandler(int sig) {
    pdft::core::fail(std::format("Received SIGSEGV({})", sig).c_str());
}

void setupSignalHandlers() {
    ::signal(SIGSEGV, sigsegvHandler);
}

}  // namespace app
