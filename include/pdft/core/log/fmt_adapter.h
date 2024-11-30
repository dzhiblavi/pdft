#pragma once

#include <rfl/json/write.hpp>
#include <spdlog/fmt/ostr.h>

#define PDFT_LOG_ADAPT_TYPE_JSON(Namespace, T)                          \
    namespace Namespace {                                               \
    inline std::ostream& operator<<(std::ostream& os, const T& value) { \
        return rfl::json::write(value, os << #T);                       \
    }                                                                   \
    }                                                                   \
    template <>                                                         \
    struct fmt::formatter<Namespace::T> : fmt::ostream_formatter {}

#define PDFT_LOG_ADAPT_TEMPLATE_JSON(Namespace, T, ...)                              \
    namespace Namespace {                                                            \
    template <typename __VA_ARGS__>                                                  \
    inline std::ostream& operator<<(std::ostream& os, const T<__VA_ARGS__>& value) { \
        return rfl::json::write(value, os << #T);                                    \
    }                                                                                \
    }                                                                                \
    template <typename __VA_ARGS__>                                                  \
    struct fmt::formatter<Namespace::T<__VA_ARGS__>> : fmt::ostream_formatter {}
