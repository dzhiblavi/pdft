#pragma once

#include "pdft/core/log/config.h"

namespace pdft::log {

void loadLogger(const std::string& name, const LoggerConfig& config);

void loadConfig(const LogConfig& config);

}  // namespace pdft::log
