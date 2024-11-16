#pragma once

#include "pdft/core/log/log.h"
#include "pdft/core/trace/time_traits.h"
#include "pdft/utils/overloaded.h"
#include "pdft/utils/source_location.h"

namespace pdft::core::trace {

namespace detail {

template <typename Duration>
inline constexpr const char* durationSuffix = "";

template <>
inline constexpr const char* durationSuffix<std::chrono::nanoseconds> = "ns";

template <>
inline constexpr const char* durationSuffix<std::chrono::microseconds> = "us";

template <>
inline constexpr const char* durationSuffix<std::chrono::milliseconds> = "ms";

template <>
inline constexpr const char* durationSuffix<std::chrono::seconds> = "s";

template <>
inline constexpr const char* durationSuffix<std::chrono::minutes> = "m";

}  // namespace detail

template <typename Duration = std::chrono::milliseconds>
inline auto makeLoggingTracer(
    const char* label, utils::SourceLocation loc, log::LoggerPtr logger = log::logger("trace")) {
    return utils::Overloaded{
        [=](DurationType duration) {
            auto dur = std::chrono::duration_cast<Duration>(duration);

            logger->trace(
                "[{}] {}{} {}:{}",
                label,
                dur.count(),
                detail::durationSuffix<Duration>,
                loc.function,
                loc.line);
        },
        [=](DurationType duration, auto& self) {
            auto dur = std::chrono::duration_cast<Duration>(duration);

            logger->trace(
                "[{}] {}{}, @{}:{}",
                label,
                dur.count(),
                detail::durationSuffix<Duration>,
                loc.function,
                loc.line);
        },
    };
}

}  // namespace pdft::core::trace

#define PDFT_LOGGING_TRACER(label, ...) \
    ::pdft::core::trace::makeLoggingTracer(label, PDFT_SOURCE_LOCATION, ##__VA_ARGS__)
