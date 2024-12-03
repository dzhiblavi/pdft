#pragma once

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <magic_enum.hpp>

#include <filesystem>
#include <ostream>
#include <type_traits>

// Format std::filesystem::path
inline std::ostream& operator<<(std::ostream& os, const std::filesystem::path& path) {
    return os << path.c_str();
}

template <>
struct fmt::formatter<std::filesystem::path> : fmt::ostream_formatter {};

// Format all enums by default
template <typename T>
requires std::is_enum_v<T>
std::ostream& operator<<(std::ostream& os, const T& value) {
    return os << magic_enum::enum_name(value);
}

// Format std::error_code by default
template <typename T>
requires std::is_enum_v<T>
struct fmt::formatter<T> : fmt::ostream_formatter {};

inline std::ostream& operator<<(std::ostream& os, const std::error_code& ec) {
    return os << ec.category().name() << ": " << ec.category().message(ec.value());
}

template <>
struct fmt::formatter<std::error_code> : fmt::ostream_formatter {};
