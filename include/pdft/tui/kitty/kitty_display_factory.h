#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/kitty/apc/apc_executor.h"
#include "pdft/tui/terminal.h"

#include "pdft/core/module/factory.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include "pdft/utils/cli/cli.h"

namespace pdft::tui {

class KittyDisplayFactory : public AbstractFactory<Display>, public Module {
 public:
    struct Config {
        bool force_unicode = false;
    };

    struct CliArgs {
        cli::opt<bool, 'U', "Force Unicode Kitty image display protocol"> force_unicode = false;
    };

    KittyDisplayFactory(
        Config config,
        CliArgs cli,
        inject<APCExecutor, "apc"> executor,
        inject<const Terminal, "terminal"> terminal);

    Arc<Display> create() const override;

 private:
    bool force_unicode_;
    Arc<APCExecutor> executor_;
    Arc<const Terminal> terminal_;
};

}  // namespace pdft::tui
