#pragma once

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <filesystem>
#include <ostream>

inline std::ostream& operator<<(std::ostream& os, const std::filesystem::path& path) {
    return os << path.c_str();
}

template <>
struct fmt::formatter<std::filesystem::path> : fmt::ostream_formatter {};
