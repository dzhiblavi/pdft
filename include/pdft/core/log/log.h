#pragma once

#include <spdlog/common.h>
#include <spdlog/logger.h>

#include <memory>

namespace pdft::core::log {

using LoggerPtr = std::shared_ptr<spdlog::logger>;

namespace level = spdlog::level;

LoggerPtr logger(const std::string& name);

LoggerPtr global();

void setGlobalLogger(LoggerPtr logger);

}  // namespace pdft::core::log
