#pragma once

#include <algorithm>
#include <source_location>
#include <string_view>

#include <fmt/core.h>

namespace pdft::core {

namespace detail {

[[noreturn]] void panic(
    std::string_view message, std::source_location location = std::source_location::current());

}  // namespace detail

template <typename... Args>
struct panic {
    panic(
        fmt::format_string<const Args&...> fmt,
        const Args&... args,
        const std::source_location& loc = std::source_location::current()) {
        detail::panic(fmt::format(fmt, args...), loc);
    }

    panic(const Args&... args, const std::source_location& loc = std::source_location::current()) {
        detail::panic(args..., loc);
    }

    template <typename T>
    [[noreturn]] operator T() const {
        std::terminate();
    }
};

template <typename... Args>
panic(fmt::format_string<const Args&...>, const Args&...) -> panic<Args...>;

template <typename Msg>
panic(const Msg&) -> panic<Msg>;

constexpr bool containsFormatPlaceholder(std::string_view str) {
    return std::ranges::any_of(str, [](char c) { return c == '{' || c == '}'; });
}

}  // namespace pdft::core

#define PDFT_VERIFY(expr, ...)                                                                    \
    do {                                                                                          \
        if (!static_cast<bool>(expr)) [[unlikely]] {                                              \
            constexpr bool containsFormatPlaceholder =                                            \
                ::pdft::core::containsFormatPlaceholder(#expr);                                   \
                                                                                                  \
            if constexpr (containsFormatPlaceholder) {                                            \
                static_assert(                                                                    \
                    !containsFormatPlaceholder,                                                   \
                    "expression should not contain format placeholders: " #expr);                 \
            } else {                                                                              \
                ::pdft::core::panic("PDFT_VERIFY failed: '" #expr "' is not true: " __VA_ARGS__); \
            }                                                                                     \
        }                                                                                         \
    } while (0)
