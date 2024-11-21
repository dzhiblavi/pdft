#pragma once

#include "pdft/media/pdf/context.h"
#include "pdft/utils/get_home_dir.h"

#include <spdlog/common.h>

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

struct LoggingConfig {
    std::filesystem::path directory = pdft::utils::getHomeDirectory() / ".local/share/pdft/";
    LogLevel level = LogLevel::Info;
};

void setupLogging(const LoggingConfig& config);

void setupMupdfLogging(const pdft::media::pdf::Context& context);

}  // namespace app
