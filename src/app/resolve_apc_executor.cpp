#include "resolve_apc_executor.h"

#include "pdft/core/apc/regular/apc_executor.h"
#include "pdft/core/apc/tmux/apc_executor.h"
#include "pdft/core/log/log.h"

namespace app {

using namespace pdft;
using namespace pdft::core::log::exported;

std::shared_ptr<core::apc::APCExecutor> resolveAPCExecutor(
    std::shared_ptr<pdft::core::io::Console> console) {
    if (auto _ = ::getenv("TMUX")) {
        log::info("Resolved TMUX APC Executor");
        return std::make_shared<core::apc::tmux::APCExecutor>(
            core::apc::Config{}, std::move(console));
    } else {
        log::info("Resolved Regular APC Executor");
        return std::make_shared<core::apc::regular::APCExecutor>(
            core::apc::Config{}, std::move(console));
    }
}

}  // namespace app
