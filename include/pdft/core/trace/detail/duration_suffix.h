#pragma once

#include <chrono>  // IWYU pragma: keep

namespace pdft::trace::detail {

template <typename Duration>
inline constexpr const char* DurationSuffix = "";

template <>
inline constexpr const char* DurationSuffix<std::chrono::nanoseconds> = "ns";

template <>
inline constexpr const char* DurationSuffix<std::chrono::microseconds> = "us";

template <>
inline constexpr const char* DurationSuffix<std::chrono::milliseconds> = "ms";

template <>
inline constexpr const char* DurationSuffix<std::chrono::seconds> = "s";

template <>
inline constexpr const char* DurationSuffix<std::chrono::minutes> = "m";

}  // namespace pdft::trace::detail
