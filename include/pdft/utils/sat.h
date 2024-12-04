#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

namespace pdft::utils {

template <std::integral T>
constexpr T add_sat(T x, T y) noexcept {
    T z;
    if (!__builtin_add_overflow(x, y, &z)) {
        return z;
    }
    if constexpr (std::is_unsigned_v<T>) {
        return std::numeric_limits<T>::max();
    } else if (x < 0) {
        return std::numeric_limits<T>::min();
    } else {
        return std::numeric_limits<T>::max();
    }
}

template <std::integral T>
constexpr T sub_sat(T x, T y) noexcept {
    T z;
    if (!__builtin_sub_overflow(x, y, &z)) {
        return z;
    }
    if constexpr (std::is_unsigned_v<T>) {
        return std::numeric_limits<T>::min();
    } else if (x < 0) {
        return std::numeric_limits<T>::min();
    } else {
        return std::numeric_limits<T>::max();
    }
}

}  // namespace pdft::utils
