#pragma once

#include <spdlog/common.h>

#include <cstdint>
#include <string>

namespace app {

enum class LogLevel {
    Trace = spdlog::level::trace,
    Debug = spdlog::level::debug,
    Info = spdlog::level::info,
    Warn = spdlog::level::warn,
    Err = spdlog::level::err,
    Critical = spdlog::level::critical,
    Off = spdlog::level::off,
};

struct SystemConfig {
    uint32_t max_event_queue_size = 10;
    bool enable_filesystem_watching = true;
};

struct Logging {
    std::string dir;
    LogLevel level = LogLevel::Info;
};

struct Config {
    SystemConfig system = {};
    Logging logging = {};
};

}  // namespace app
