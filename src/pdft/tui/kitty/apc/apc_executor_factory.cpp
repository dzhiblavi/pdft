#include "pdft/tui/kitty/apc/apc_executor_factory.h"

#include "pdft/tui/kitty/apc/regular_apc_executor.h"
#include "pdft/tui/kitty/apc/tmux_apc_executor.h"

#include "pdft/core/log.h"
#include "pdft/core/module/register.h"

namespace pdft::tui {

APCExecutorFactory::APCExecutorFactory(APCExecutorConfig config) : config_(config) {}

Arc<APCExecutor> APCExecutorFactory::create() const {
    if (::getenv("TMUX") != nullptr) {
        log::info(this, "loading tmux executor because TMUX environment variable is set");
        return std::make_shared<TmuxAPCExecutor>(config_);
    }

    log::info(this, "loading regular executor");
    return std::make_shared<RegularAPCExecutor>(config_);
}

}  // namespace pdft::tui

PDFT_REGISTER_FACTORY(pdft::tui::APCExecutor, pdft::tui::APCExecutorFactory);
