#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/kitty/apc/apc_executor.h"
#include "pdft/tui/kitty/kitty_display_base.h"
#include "pdft/tui/terminal.h"

#include "pdft/core/factory.h"
#include "pdft/core/module/module.h"

#include "pdft/core/cli/cli.h"

namespace pdft::tui {

class KittyDisplayFactory : public AbstractFactory<Display>, public Module {
 public:
    struct Config {
        bool force_unicode = false;
        KittyDisplayBase::Config base{};
    };

    struct CliArgs {
        cli::opt<bool>                                            //
            ::desc<"Force Unicode Kitty image display protocol">  //
            ::shrt<'U'> force_unicode = false;
    };

    KittyDisplayFactory(
        Config config, CliArgs cli, Arc<APCExecutor> executor, Arc<const Terminal> terminal);

    Arc<Display> create() const override;

 private:
    Config config_;
    CliArgs cli_;
    Arc<APCExecutor> executor_;
    Arc<const Terminal> terminal_;
};

}  // namespace pdft::tui
