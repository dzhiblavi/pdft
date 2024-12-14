#pragma once

#include <rfl/Field.hpp>
#include <rfl/Variant.hpp>

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <map>
#include <string>

namespace pdft::log {

enum class LogLevel {
    Trace = spdlog::level::trace,
    Debug = spdlog::level::debug,
    Info = spdlog::level::info,
    Warn = spdlog::level::warn,
    Err = spdlog::level::err,
    Critical = spdlog::level::critical,
    Off = spdlog::level::off,
};

enum class SyncMode {
    Sync,
    AsyncBlocking,
    AsyncOverflow,
};

struct StdoutSink {
    bool cerr = false;
    bool colors = false;
};

struct SimpleFileSink {
    std::string path;
};

struct RotatingFileSink {
    static constexpr size_t DefaultMaxSize = 1ULL << 20ULL;
    static constexpr size_t DefaultMaxNumFiles = 5;

    std::string path;
    size_t max_size = DefaultMaxSize;
    size_t max_num_files = DefaultMaxNumFiles;
};

struct DailyFileSink {
    std::string path;
    size_t hour = 0;
    size_t minute = 0;
};

using SinkConfig = rfl::Variant<
    rfl::Field<"stdout", StdoutSink>,
    rfl::Field<"file", SimpleFileSink>,
    rfl::Field<"rot_file", RotatingFileSink>,
    rfl::Field<"daily_file", DailyFileSink> >;

struct LoggerConfig {
    LogLevel level = LogLevel::Info;
    std::string format = "";
    SyncMode sync_mode = SyncMode::Sync;
    bool force_flush = true;
    std::vector<SinkConfig> sinks;
};

struct ThreadPoolConfig {
    static constexpr size_t DefaultQueueSize = 8192;

    size_t queue_size = DefaultQueueSize;
    size_t num_threads = 1;
};

struct LogConfig {
    ThreadPoolConfig thread_pool = {};
    std::map<std::string, LoggerConfig> loggers;
};

}  // namespace pdft::log
