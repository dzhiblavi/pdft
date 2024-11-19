#include "resolve_apc_executor.h"

#include "pdft/core/log/log.h"
#include "pdft/core/terminal/regular/apc_executor.h"
#include "pdft/core/terminal/tmux/apc_executor.h"

namespace app {

using namespace pdft;

std::shared_ptr<core::terminal::APCExecutor> resolveAPCExecutor() {
    if (auto _ = ::getenv("TMUX")) {
        core::log::global()->info("Resolved TMUX APC Executor");
        return std::make_shared<core::terminal::tmux::APCExecutor>();
    } else {
        core::log::global()->info("Resolved Regular APC Executor");
        return std::make_shared<core::terminal::regular::APCExecutor>();
    }
}

}  // namespace app
