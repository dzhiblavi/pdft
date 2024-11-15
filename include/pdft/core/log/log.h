#pragma once

#include <spdlog/common.h>
#include <spdlog/logger.h>

#include <memory>

namespace pdft::core::log {

using LoggerPtr = std::shared_ptr<spdlog::logger>;

LoggerPtr getLogger(const std::string& name);

LoggerPtr getGlobalLogger();

void setGlobalLogger(LoggerPtr logger);

}  // namespace pdft::core::log

#define LOG(logger, severity, ...) logger->log(spdlog::level::severity, ##__VA_ARGS__)

#define LOG_GLOBAL(severity, ...) LOG(::pdft::core::log::getGlobalLogger(), severity, ##__VA_ARGS__)

#define GLOG LOG_GLOBAL
