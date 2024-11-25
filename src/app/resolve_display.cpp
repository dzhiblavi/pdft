#include "resolve_display.h"

#include "pdft/tui/display/kitty/apc/regular/executor.h"
#include "pdft/tui/display/kitty/apc/tmux/executor.h"
#include "pdft/tui/display/kitty/native_display.h"
#include "pdft/tui/display/kitty/unicode_display.h"

#include "pdft/core/log/log.h"

namespace app {

using namespace pdft;

namespace {

std::shared_ptr<tui::display::kitty::apc::Executor> resolveAPCExecutor(
    std::shared_ptr<pdft::core::io::Console> console) {
    if (auto _ = ::getenv("TMUX")) {
        log::info("Resolved TMUX APC Executor");
        return std::make_shared<tui::display::kitty::apc::tmux::Executor>(
            tui::display::kitty::apc::Config{}, std::move(console));
    } else {
        log::info("Resolved Regular APC Executor");
        return std::make_shared<tui::display::kitty::apc::regular::Executor>(
            tui::display::kitty::apc::Config{}, std::move(console));
    }
}

}  // namespace

std::shared_ptr<pdft::tui::display::Display> resolveDisplay(
    std::shared_ptr<pdft::core::io::Console> console) {
    auto executor = resolveAPCExecutor(console);

    if (auto _ = ::getenv("TMUX")) {
        log::info("Resolved Unicode Kitty display");
        return std::make_shared<tui::display::kitty::UnicodeDisplay>(
            std::move(executor), std::move(console));
    } else {
        log::info("Resolved Native Kitty display");
        return std::make_shared<tui::display::kitty::NativeDisplay>(
            std::move(executor), std::move(console));
    }
}

}  // namespace app
