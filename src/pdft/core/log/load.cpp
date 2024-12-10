#include "pdft/core/log/load.h"
#include "pdft/core/log.h"

#include "pdft/utils/overloaded.h"

#include <rfl/visit.hpp>

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace pdft::log {

namespace {

constexpr auto GLOBAL_LOGGER_NAME = "global";

spdlog::sink_ptr loadSink(const SinkConfig& config) {
    return rfl::visit(
        utils::Overloaded{
            [](const rfl::Field<"stdout", StdoutSink>& v) -> spdlog::sink_ptr {
                auto&& sink = v.value();
                if (sink.cerr) {
                    if (sink.colors) {
                        return std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
                    }
                    return std::make_shared<spdlog::sinks::stderr_sink_mt>();
                }
                if (sink.colors) {
                    return std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                }
                return std::make_shared<spdlog::sinks::stdout_sink_mt>();
            },
            [](const rfl::Field<"file", SimpleFileSink>& v) -> spdlog::sink_ptr {
                auto&& sink = v.value();
                return std::make_shared<spdlog::sinks::basic_file_sink_mt>(sink.path);
            },
            [](const rfl::Field<"rot_file", RotatingFileSink>& v) -> spdlog::sink_ptr {
                auto&& sink = v.value();
                return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    sink.path, sink.max_size, sink.max_num_files);
            },
            [](const rfl::Field<"daily_file", DailyFileSink>& v) -> spdlog::sink_ptr {
                auto&& sink = v.value();
                return std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                    sink.path, sink.hour, sink.minute);
            },
        },
        config);
}

}  // namespace

void loadLogger(const std::string& name, const LoggerConfig& config) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.reserve(config.sinks.size());
    std::transform(config.sinks.begin(), config.sinks.end(), std::back_inserter(sinks), loadSink);

    auto logger = [&]() -> Arc<spdlog::logger> {
        switch (config.sync_mode) {
            using enum SyncMode;
            using enum spdlog::async_overflow_policy;

            case Sync:
                return std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
            case AsyncBlocking:
            case AsyncOverflow:
                return std::make_shared<spdlog::async_logger>(
                    name,
                    sinks.begin(),
                    sinks.end(),
                    spdlog::thread_pool(),
                    config.sync_mode == AsyncBlocking ? block : overrun_oldest);
        }

        return {};
    }();

    logger->set_level(static_cast<spdlog::level::level_enum>(config.level));

    if (!config.format.empty()) {
        logger->set_pattern(config.format);
    }

    if (config.force_flush) {
        logger->flush_on(spdlog::level::trace);
    }

    spdlog::register_logger(logger);
}

void loadConfig(const LogConfig& config) {
    spdlog::init_thread_pool(config.thread_pool.queue_size, config.thread_pool.num_threads);

    for (const auto& [name, conf] : config.loggers) {
        loadLogger(name, conf);
    }

    if (auto global = spdlog::get(GLOBAL_LOGGER_NAME)) {
        setGlobalLogger(global);
    }
}

}  // namespace pdft::log
