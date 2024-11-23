#pragma once

#include <source_location>
#include <spdlog/common.h>
#include <spdlog/logger.h>

#include <memory>

namespace pdft::core::log::impl {

using LoggerPtr = std::shared_ptr<spdlog::logger>;

using Level = spdlog::level::level_enum;

LoggerPtr global();

template <typename... Args>
struct log {
    log(LoggerPtr logger,
        Level level,
        std::format_string<Args...> fmt,
        Args&&... args,
        const std::source_location loc = std::source_location::current()) {
        auto spdlog_loc = spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());
        logger->log(spdlog_loc, level, std::format(fmt, std::forward<Args>(args)...));
    }

    log(LoggerPtr logger,
        Level level,
        Args&&... args,
        const std::source_location loc = std::source_location::current()) {
        auto spdlog_loc = spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());
        logger->log(spdlog_loc, level, std::forward<Args>(args)...);
    }
};

template <typename... Args>
log(LoggerPtr, Level, std::format_string<Args...>, Args&&...) -> log<Args...>;

template <typename Msg>
log(LoggerPtr, Level, Msg&&) -> log<Msg>;

#define PDFT_DEFINE_LOG_FOR_LEVEL(level)                                                          \
    template <typename... Args>                                                                   \
    struct level {                                                                                \
        level(                                                                                    \
            LoggerPtr logger,                                                                     \
            std::format_string<Args...> fmt,                                                      \
            Args&&... args,                                                                       \
            const std::source_location loc = std::source_location::current()) {                   \
            auto spdlog_loc =                                                                     \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());             \
            logger->log(spdlog_loc, Level::level, std::format(fmt, std::forward<Args>(args)...)); \
        }                                                                                         \
                                                                                                  \
        level(                                                                                    \
            LoggerPtr logger,                                                                     \
            Args&&... args,                                                                       \
            const std::source_location loc = std::source_location::current()) {                   \
            auto spdlog_loc =                                                                     \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());             \
            logger->log(spdlog_loc, Level::level, std::forward<Args>(args)...);                   \
        }                                                                                         \
                                                                                                  \
        level(                                                                                    \
            std::format_string<Args...> fmt,                                                      \
            Args&&... args,                                                                       \
            const std::source_location loc = std::source_location::current()) {                   \
            auto spdlog_loc =                                                                     \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());             \
            global()->log(                                                                        \
                spdlog_loc, Level::level, std::format(fmt, std::forward<Args>(args)...));         \
        }                                                                                         \
                                                                                                  \
        level(Args&&... args, const std::source_location loc = std::source_location::current()) { \
            auto spdlog_loc =                                                                     \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());             \
            global()->log(spdlog_loc, Level::level, std::forward<Args>(args)...);                 \
        }                                                                                         \
    };                                                                                            \
                                                                                                  \
    template <typename... Args>                                                                   \
    level(std::format_string<Args...>, Args&&...) -> level<Args...>;                              \
    template <typename Msg>                                                                       \
    level(Msg&&) -> level<Msg>;                                                                   \
    template <typename... Args>                                                                   \
    level(LoggerPtr, std::format_string<Args...>, Args&&...) -> level<Args...>;                   \
    template <typename Msg>                                                                       \
    level(LoggerPtr, Msg&&)->level<Msg>

PDFT_DEFINE_LOG_FOR_LEVEL(trace);
PDFT_DEFINE_LOG_FOR_LEVEL(debug);
PDFT_DEFINE_LOG_FOR_LEVEL(info);
PDFT_DEFINE_LOG_FOR_LEVEL(warn);
PDFT_DEFINE_LOG_FOR_LEVEL(err);
PDFT_DEFINE_LOG_FOR_LEVEL(critical);
#undef PDFT_DEFINE_LOG_FOR_LEVEL

}  // namespace pdft::core::log::impl
