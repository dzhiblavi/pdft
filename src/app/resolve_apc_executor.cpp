#include "resolve_apc_executor.h"

#include "pdft/core/apc/regular/apc_executor.h"
#include "pdft/core/apc/tmux/apc_executor.h"
#include "pdft/core/log/log.h"

namespace app {

using namespace pdft;

std::shared_ptr<core::apc::APCExecutor> resolveAPCExecutor(
    std::shared_ptr<pdft::core::io::Console> console) {
    if (auto _ = ::getenv("TMUX")) {
        core::log::global()->info("Resolved TMUX APC Executor");
        return std::make_shared<core::apc::tmux::APCExecutor>(std::move(console));
    } else {
        core::log::global()->info("Resolved Regular APC Executor");
        return std::make_shared<core::apc::regular::APCExecutor>(std::move(console));
    }
}

}  // namespace app
