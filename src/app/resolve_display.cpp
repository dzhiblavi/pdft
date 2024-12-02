#include "resolve_display.h"

#include "pdft/tui/display/kitty/apc/regular/executor.h"
#include "pdft/tui/display/kitty/apc/tmux/executor.h"
#include "pdft/tui/display/kitty/native_display.h"
#include "pdft/tui/display/kitty/unicode_display.h"

#include "pdft/core/log/log.h"

namespace app {

using namespace pdft;

namespace {

Arc<tui::display::kitty::apc::Executor> resolveAPCExecutor() {
    if (auto _ = ::getenv("TMUX")) {
        log::info("Resolved TMUX APC Executor");
        return std::make_shared<tui::display::kitty::apc::tmux::Executor>(
            tui::display::kitty::apc::Config{});
    } else {
        log::info("Resolved Regular APC Executor");
        return std::make_shared<tui::display::kitty::apc::regular::Executor>(
            tui::display::kitty::apc::Config{});
    }
}

}  // namespace

Arc<pdft::tui::display::Display> resolveDisplay(
    const DisplayConfig& config, Arc<const pdft::tui::terminal::Terminal> terminal) {
    auto executor = resolveAPCExecutor();

    if (auto _ = ::getenv("TMUX") || config.force_unicode_kitty_display) {
        log::info("Resolved Unicode Kitty display");
        return std::make_shared<tui::display::kitty::UnicodeDisplay>(
            std::move(executor), std::move(terminal));
    } else {
        log::info("Resolved Native Kitty display");
        return std::make_shared<tui::display::kitty::NativeDisplay>(
            std::move(executor), std::move(terminal));
    }
}

}  // namespace app
