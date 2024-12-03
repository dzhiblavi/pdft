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

template <>
struct fmt::formatter<std::error_code> {
    constexpr decltype(auto) parse(format_parse_context& ctx) {
        return ctx.end();
    }

    template <typename FormatContext>
    decltype(auto) format(const std::error_code& ec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "ec(cat={},msg={})", ec.category().name(), ec.message());
    }
};
