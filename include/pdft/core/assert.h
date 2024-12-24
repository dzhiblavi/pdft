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
struct panic {  // NOLINT
    panic(
        fmt::format_string<const Args&...> fmt,
        const Args&... args,
        const std::source_location& loc = std::source_location::current()) {
        detail::panic(fmt::format(fmt, args...), loc);
    }

    panic(std::string_view msg, const std::source_location& loc = std::source_location::current()) {
        detail::panic(msg, loc);
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

template <typename T, typename... Args>
struct verify {  // NOLINT
    verify(
        T value,
        fmt::format_string<const Args&...> fmt,
        const Args&... args,
        const std::source_location& loc = std::source_location::current())
        : val_(std::forward<T>(value)) {
        if (!static_cast<bool>(val_)) {
            detail::panic(fmt::format(fmt, args...), loc);
        }
    }

    verify(
        T value,
        std::string_view msg,
        const std::source_location& loc = std::source_location::current())
        : val_(std::forward<T>(value)) {
        if (!static_cast<bool>(val_)) {
            detail::panic(msg, loc);
        }
    }

    verify(T value, const std::source_location& loc = std::source_location::current())
        : val_(std::forward<T>(value)) {
        if (!static_cast<bool>(val_)) {
            detail::panic("verify() failed", loc);
        }
    }

    operator T() && {
        return std::forward<T>(val_);
    }

 private:
    T val_;
};

template <typename T, typename... Args>
verify(T&&, fmt::format_string<const Args&...>, const Args&...) -> verify<T&&, Args...>;

template <typename T, typename Msg>
verify(T&&, const Msg&) -> verify<T&&, Msg>;

}  // namespace pdft

#ifndef NDEBUG
// NOLINTNEXTLINE
#define PDFT_VERIFY_DBG(...) (void)verify(__VA_ARGS__)
#else
#define PDFT_VERIFY_DBG(...)
#endif
