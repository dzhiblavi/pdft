#pragma once

#include <chrono>

namespace pdft::core::trace::detail {

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

}  // namespace pdft::core::trace::detail
