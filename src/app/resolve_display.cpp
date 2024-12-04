#include "resolve_display.h"

#include "pdft/tui/kitty/apc/regular_apc_executor.h"
#include "pdft/tui/kitty/apc/tmux_apc_executor.h"
#include "pdft/tui/kitty/native_display.h"
#include "pdft/tui/kitty/unicode_display.h"

#include "pdft/core/log.h"

namespace app {

using namespace pdft;

namespace {

Arc<tui::APCExecutor> resolveAPCExecutor() {
    if (auto _ = ::getenv("TMUX")) {
        log::info("Resolved TMUX APC Executor");
        return std::make_shared<tui::TmuxAPCExecutor>(tui::APCExecutorConfig{});
    } else {
        log::info("Resolved Regular APC Executor");
        return std::make_shared<tui::RegularAPCExecutor>(tui::APCExecutorConfig{});
    }
}

}  // namespace

Arc<pdft::tui::Display> resolveDisplay(
    const DisplayConfig& config, Arc<const pdft::tui::Terminal> terminal) {
    auto executor = resolveAPCExecutor();

    if (auto _ = ::getenv("TMUX") || config.force_unicode_kitty_display) {
        log::info("Resolved Unicode Kitty display");
        return std::make_shared<tui::UnicodeKittyDisplay>(std::move(executor), std::move(terminal));
    } else {
        log::info("Resolved Native Kitty display");
        return std::make_shared<tui::NativeKittyDisplay>(std::move(executor), std::move(terminal));
    }
}

}  // namespace app
