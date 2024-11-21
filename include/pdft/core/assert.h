#pragma once

#include <source_location>
#include <string_view>

namespace pdft::core {

[[noreturn]] void fail(
    std::string_view message, std::source_location location = std::source_location::current());

}  // namespace pdft::core

#define PDFT_VERIFY(expr)                                                                        \
    do {                                                                                         \
        if (!static_cast<bool>(expr)) [[unlikely]] {                                             \
            ::pdft::core::fail(                                                                  \
                "PDFT_VERIFY failed: '" #expr "' is not true", std::source_location::current()); \
        }                                                                                        \
    } while (0)
