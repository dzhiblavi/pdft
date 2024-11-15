#pragma once

#include "pdft/core/log/log.h"
#include "pdft/core/trace/time_traits.h"
#include "pdft/utils/overloaded.h"

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
inline auto makeLoggingTracer(const char* label, log::LoggerPtr logger = log::getLogger("trace")) {
    return utils::Overloaded{
        [=](DurationType duration) {
            auto dur = std::chrono::duration_cast<Duration>(duration);
            LOG(logger,
                trace,
                "[{}] took {}{}",
                label,
                dur.count(),
                detail::durationSuffix<Duration>);
        },
        [=](DurationType duration, auto& self) {
            auto dur = std::chrono::duration_cast<Duration>(duration);
            LOG(logger,
                trace,
                "@{} [{}] took {}{}",
                self.depth(),
                label,
                dur.count(),
                detail::durationSuffix<Duration>);
        },
    };
}

}  // namespace pdft::core::trace
