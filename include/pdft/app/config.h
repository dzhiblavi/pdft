#pragma once

#include "pdft/core/log/config.h"
#include "pdft/core/module/config.h"

namespace pdft::app {

struct Config {
    log::LogConfig logging;
    module::ModulesConfig modules;
};

}  // namespace pdft::app
