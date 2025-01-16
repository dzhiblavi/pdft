#pragma once

#include "platform/log/config.h"
#include "platform/module/config.h"

namespace pdft::rt {

struct Config {
    platform::log::LogConfig logging;
    platform::rt::ModulesConfig modules;
};

}  // namespace pdft::rt
