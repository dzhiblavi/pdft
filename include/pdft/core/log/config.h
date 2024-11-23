#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <map>
#include <string>
#include <variant>

namespace pdft::core::log {

struct StdoutSink {
    bool cerr = false;
    bool colors = false;
};

struct SimpleFileSink {
    std::string path;
};

struct RotatingFileSink {
    std::string path;
    size_t max_size = 1ULL << 20ULL;
    size_t max_num_files = 5;
};

struct DailyFileSink {
    std::string path;
    size_t hour = 0;
    size_t minute = 0;
};

using SinkConfig = std::variant<StdoutSink, SimpleFileSink, RotatingFileSink, DailyFileSink>;

enum SyncMode {
    Sync,
    AsyncBlocking,
    AsyncOverflow,
};

struct LoggerConfig {
    spdlog::level::level_enum level = spdlog::level::level_enum::info;
    std::string format = "";
    SyncMode sync_mode = SyncMode::Sync;
    bool force_flush = true;
    std::vector<SinkConfig> sinks;
};

struct ThreadPoolConfig {
    size_t queue_size = 8192;
    size_t num_threads = 1;
};

struct LogConfig {
    ThreadPoolConfig thread_pool = {};
    std::map<std::string, LoggerConfig> loggers;
};

}  // namespace pdft::core::log
