#pragma once

#include "pdft/core/result/type/error.h"

#include <expected>
#include <format>

namespace pdft::result {

inline auto err(Error error) {
    return std::unexpected(error);
}

template <typename... Args>
auto err(std::format_string<const Args&...> fmt, const Args&... args) {
    return std::unexpected(std::format(fmt, args...));
}

}  // namespace pdft::result
