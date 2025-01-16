#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/kitty/apc/apc_executor.h"
#include "pdft/tui/kitty/kitty_display_base.h"
#include "pdft/tui/terminal.h"

#include "platform/cli/cli.h"
#include "platform/core/factory.h"
#include "platform/module/module.h"

namespace pdft::tui {

class KittyDisplayFactory : public platform::AbstractFactory<Display>, public platform::rt::Module {
 public:
    struct Config {
        bool force_unicode = false;
        KittyDisplayBase::Config base{};
    };

    struct CliArgs {
        platform::rt::cli_opt<bool>                               //
            ::desc<"Force Unicode Kitty image display protocol">  //
            ::shrt<'U'> force_unicode = false;
    };

    KittyDisplayFactory(
        Config config,
        CliArgs cli,
        platform::Arc<APCExecutor> executor,
        platform::Arc<const Terminal> terminal);

    platform::Arc<Display> create() const override;

 private:
    Config config_;
    CliArgs cli_;
    platform::Arc<APCExecutor> executor_;
    platform::Arc<const Terminal> terminal_;
};

}  // namespace pdft::tui
