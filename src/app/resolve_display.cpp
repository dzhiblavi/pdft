#include "resolve_display.h"

#include "pdft/core/log/log.h"
#include "pdft/view/image/kitty/unicode_display.h"

namespace app {

using namespace pdft;

std::shared_ptr<pdft::view::image::Display> resolveDisplay(
    std::shared_ptr<pdft::core::terminal::Terminal> terminal,
    std::shared_ptr<pdft::core::terminal::APCExecutor> executor) {
    if (auto _ = ::getenv("TMUX")) {
        core::log::global()->info("Resolved Unicode Kitty display");
        return std::make_shared<view::image::kitty::UnicodeDisplay>(
            std::move(terminal), std::move(executor));
    } else {
        // core::log::global()->info("Resolved Native Kitty display");
        // return std::make_shared<view::image::kitty::NativeDisplay>(
        // std::move(terminal), std::move(executor));

        core::log::global()->info(
            "Resolved Unicode Kitty display. Native display will be used in these circumstances in "
            "the future.");
        return std::make_shared<view::image::kitty::UnicodeDisplay>(
            std::move(terminal), std::move(executor));
    }
}

}  // namespace app
