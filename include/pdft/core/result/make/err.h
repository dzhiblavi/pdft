#pragma once

#include <expected>
#include <format>

namespace pdft::result {

template <typename E>
inline auto err(E error) {
    return std::unexpected(std::move(error));
}

template <typename... Args>
auto err(std::format_string<const Args&...> fmt, const Args&... args) {
    return std::unexpected(std::format(fmt, args...));
}

}  // namespace pdft::result
