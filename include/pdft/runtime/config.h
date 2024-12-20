#pragma once

#include "pdft/core/log/config.h"
#include "pdft/runtime/module/config.h"

namespace pdft::rt {

struct Config {
    log::LogConfig logging;
    ModulesConfig modules;
};

}  // namespace pdft::rt
