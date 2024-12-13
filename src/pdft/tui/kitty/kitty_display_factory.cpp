#include "pdft/tui/kitty/kitty_display_factory.h"
#include "pdft/tui/kitty/native_kitty_display.h"
#include "pdft/tui/kitty/unicode_kitty_display.h"

#include "pdft/core/module/register.h"

#include "pdft/utils/cli/register.h"

namespace pdft::tui {

KittyDisplayFactory::KittyDisplayFactory(
    Config config,
    CliArgs cli,
    inject<APCExecutor, "apc"> executor,
    inject<const Terminal, "terminal"> terminal)
    : force_unicode_(cli.force_unicode || config.force_unicode)
    , executor_(std::move(executor))
    , terminal_(std::move(terminal)) {}

Arc<Display> KittyDisplayFactory::create() const {
    if (force_unicode_ || ::getenv("SSH_TTY") || ::getenv("TMUX")) {
        log::info(this, "using unicode placement display");
        return std::make_shared<UnicodeKittyDisplay>(executor_, terminal_);
    }

    log::info(this, "using native kitty display");
    return std::make_shared<NativeKittyDisplay>(executor_, terminal_);
}

}  // namespace pdft::tui

PDFT_REGISTER_FACTORY(pdft::tui::Display, pdft::tui::KittyDisplayFactory);
PDFT_REGISTER_CLI_ARGS(pdft::tui::KittyDisplayFactory::CliArgs);
