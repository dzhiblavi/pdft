#pragma once

#include <algorithm>
#include <format>
#include <source_location>
#include <string_view>

namespace pdft::core {

[[noreturn]] void fail(
    std::string_view message, std::source_location location = std::source_location::current());

constexpr bool containsFormatPlaceholder(std::string_view str) {
    return std::ranges::any_of(str, [](char c) { return c == '{' || c == '}'; });
}

}  // namespace pdft::core

#define PDFT_VERIFY(expr, ...)                                                                 \
    do {                                                                                       \
        if (!static_cast<bool>(expr)) [[unlikely]] {                                           \
            constexpr bool containsFormatPlaceholder =                                         \
                ::pdft::core::containsFormatPlaceholder(#expr);                                \
                                                                                               \
            if constexpr (containsFormatPlaceholder) {                                         \
                static_assert(                                                                 \
                    !containsFormatPlaceholder,                                                \
                    "expression should not contain format placeholders: " #expr);              \
            } else {                                                                           \
                ::pdft::core::fail(                                                            \
                    std::format("PDFT_VERIFY failed: '" #expr "' is not true: " __VA_ARGS__)); \
            }                                                                                  \
        }                                                                                      \
    } while (0)
