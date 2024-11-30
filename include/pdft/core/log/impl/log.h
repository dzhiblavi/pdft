#pragma once

#include <source_location>
#include <spdlog/common.h>
#include <spdlog/logger.h>

#include <memory>

namespace pdft::core::log::impl {

using LoggerPtr = std::shared_ptr<spdlog::logger>;

LoggerPtr global();

using Level = spdlog::level::level_enum;

template <typename T>
concept WithLogger = requires(const T& t) {
    { t.getLogger() } -> std::same_as<LoggerPtr>;
};

template <typename T>
concept AsLogger =
    std::same_as<T, LoggerPtr> || WithLogger<T> || WithLogger<std::remove_pointer_t<T>>;

LoggerPtr getLogger(const WithLogger auto& x) {
    return x.getLogger();
}

LoggerPtr getLogger(const WithLogger auto* x) {
    return x->getLogger();
}

inline LoggerPtr getLogger(LoggerPtr logger) {
    return logger;
}

template <AsLogger Logger, typename... Args>
struct log {
    log(const Logger& logger,
        Level level,
        fmt::format_string<const Args&...> fmt,
        const Args&... args,
        const std::source_location loc = std::source_location::current()) {
        auto spdlog_loc = spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());
        getLogger(logger)->log(spdlog_loc, level, fmt::format(fmt, args...));
    }

    log(const Logger& logger,
        Level level,
        const Args&... args,
        const std::source_location loc = std::source_location::current()) {
        auto spdlog_loc = spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());
        getLogger(logger)->log(spdlog_loc, level, args...);
    }
};

template <AsLogger Logger, typename... Args>
log(const Logger&, Level, fmt::format_string<const Args&...>, const Args&...)
    -> log<Logger, Args...>;

template <AsLogger Logger, typename Msg>
log(const Logger&, Level, const Msg&) -> log<Logger, Msg>;

#define PDFT_DEFINE_LOG_FOR_LEVEL(level)                                                    \
    template <AsLogger Logger, typename... Args>                                            \
    struct level {                                                                          \
        level(                                                                              \
            const Logger& logger,                                                           \
            fmt::format_string<const Args&...> fmt,                                         \
            const Args&... args,                                                            \
            const std::source_location loc = std::source_location::current()) {             \
            auto spdlog_loc =                                                               \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());       \
            getLogger(logger)->log(spdlog_loc, Level::level, fmt::format(fmt, args...));    \
        }                                                                                   \
        level(                                                                              \
            const Logger& logger,                                                           \
            const Args&... args,                                                            \
            const std::source_location loc = std::source_location::current()) {             \
            auto spdlog_loc =                                                               \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());       \
            getLogger(logger)->log(spdlog_loc, Level::level, args...);                      \
        }                                                                                   \
        level(                                                                              \
            fmt::format_string<const Args&...> fmt,                                         \
            const Args&... args,                                                            \
            const std::source_location loc = std::source_location::current()) {             \
            auto spdlog_loc =                                                               \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());       \
            global()->log(spdlog_loc, Level::level, fmt::format(fmt, args...));             \
        }                                                                                   \
        level(                                                                              \
            const Args&... args,                                                            \
            const std::source_location loc = std::source_location::current()) {             \
            auto spdlog_loc =                                                               \
                spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());       \
            global()->log(spdlog_loc, Level::level, args...);                               \
        }                                                                                   \
    };                                                                                      \
                                                                                            \
    template <typename... Args>                                                             \
    level(fmt::format_string<const Args&...>, const Args&...) -> level<LoggerPtr, Args...>; \
    template <typename Msg>                                                                 \
    level(const Msg&) -> level<LoggerPtr, Msg>;                                             \
    template <AsLogger Logger, typename... Args>                                            \
    level(const Logger&, fmt::format_string<const Args&...>, const Args&...)                \
        -> level<Logger, Args...>;                                                          \
    template <AsLogger Logger, typename Msg>                                                \
    level(const Logger&, const Msg&)->level<Logger, Msg>

PDFT_DEFINE_LOG_FOR_LEVEL(trace);
PDFT_DEFINE_LOG_FOR_LEVEL(debug);
PDFT_DEFINE_LOG_FOR_LEVEL(info);
PDFT_DEFINE_LOG_FOR_LEVEL(warn);
PDFT_DEFINE_LOG_FOR_LEVEL(err);
PDFT_DEFINE_LOG_FOR_LEVEL(critical);
#undef PDFT_DEFINE_LOG_FOR_LEVEL

}  // namespace pdft::core::log::impl
