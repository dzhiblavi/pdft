#pragma once

#include <fmt/core.h>

#include <exception>
#include <source_location>
#include <string_view>

namespace pdft {

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

}  // namespace pdft

#define PDFT_VERIFY(expr, ...)                                                          \
    do {                                                                                \
        if (!static_cast<bool>(expr)) [[unlikely]] {                                    \
            ::pdft::panic("PDFT_VERIFY failed: '" #expr "' is not true: " __VA_ARGS__); \
        }                                                                               \
    } while (0)

#ifndef NDEBUG
#define PDFT_VERIFY_DBG PDFT_VERIFY
#else
#define PDFT_VERIFY_DBG(...)
#endif
