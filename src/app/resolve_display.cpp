#include "resolve_display.h"

#include "pdft/core/log/log.h"
#include "pdft/display/kitty/native_display.h"
#include "pdft/display/kitty/unicode_display.h"

namespace app {

using namespace pdft;

std::shared_ptr<pdft::display::Display> resolveDisplay(
    std::shared_ptr<pdft::core::terminal::Terminal> terminal,
    std::shared_ptr<pdft::core::terminal::APCExecutor> executor,
    std::shared_ptr<pdft::core::io::Console> console) {
    if (auto _ = ::getenv("TMUX")) {
        core::log::global()->info("Resolved Unicode Kitty display");
        return std::make_shared<display::kitty::UnicodeDisplay>(
            std::move(terminal), std::move(executor), std::move(console));
    } else {
        core::log::global()->info("Resolved Native Kitty display");
        return std::make_shared<display::kitty::NativeDisplay>(
            std::move(terminal), std::move(executor), std::move(console));
    }
}

}  // namespace app
