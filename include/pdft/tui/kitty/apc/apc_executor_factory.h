#pragma once

#include "pdft/tui/kitty/apc/apc_executor.h"

#include "platform/core/factory.h"
#include "platform/module/module.h"

namespace pdft::tui {

class APCExecutorFactory : public platform::AbstractFactory<APCExecutor>,
                           public platform::rt::Module {
 public:
    using Config = APCExecutorConfig;

    explicit APCExecutorFactory(APCExecutorConfig config);

    platform::Arc<APCExecutor> create() const override;

 private:
    APCExecutorConfig config_;
};

}  // namespace pdft::tui
