#include "pdft/tui/kitty/kitty_display_factory.h"
#include "pdft/tui/kitty/native_kitty_display.h"
#include "pdft/tui/kitty/unicode_kitty_display.h"

#include "pdft/core/module/register.h"

#include "pdft/core/cli/register.h"

namespace pdft::tui {

KittyDisplayFactory::KittyDisplayFactory(
    Config config,
    CliArgs cli,
    inject<APCExecutor, "apc"> executor,
    inject<const Terminal, "terminal"> terminal)
    : config_(config)
    , cli_(cli)
    , executor_(std::move(executor))
    , terminal_(std::move(terminal)) {}

Arc<Display> KittyDisplayFactory::create() const {
    bool force_unicode = cli_.force_unicode.get() || config_.force_unicode;

    if (force_unicode || ::getenv("SSH_TTY") || ::getenv("TMUX")) {
        log::info(this, "using unicode placement display");
        return std::make_shared<UnicodeKittyDisplay>(config_.base, executor_, terminal_);
    }

    log::info(this, "using native kitty display");
    return std::make_shared<NativeKittyDisplay>(config_.base, executor_, terminal_);
}

}  // namespace pdft::tui

PDFT_REGISTER_FACTORY(pdft::tui::Display, pdft::tui::KittyDisplayFactory);
PDFT_REGISTER_CLI_ARGS(pdft::tui::KittyDisplayFactory::CliArgs);
