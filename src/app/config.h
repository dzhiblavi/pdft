#pragma once

#include "pdft/core/log/config.h"
#include "pdft/core/module/config.h"

namespace app {

struct Config {
    pdft::log::LogConfig logging;
    pdft::ModulesConfig modules;
};

}  // namespace app
