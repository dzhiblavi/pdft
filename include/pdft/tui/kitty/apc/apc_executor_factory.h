#pragma once

#include "pdft/tui/kitty/apc/apc_executor.h"

#include "pdft/core/module/factory.h"
#include "pdft/core/module/module.h"

namespace pdft::tui {

class APCExecutorFactory : public AbstractFactory<APCExecutor>, public Module {
 public:
    using Config = APCExecutorConfig;

    explicit APCExecutorFactory(APCExecutorConfig config);

    Arc<APCExecutor> create() const override;

 private:
    APCExecutorConfig config_;
};

}  // namespace pdft::tui
