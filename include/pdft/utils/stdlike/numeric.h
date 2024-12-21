#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

namespace pdft::stdlike {

template <std::integral T>
constexpr T add_sat(T x, T y) noexcept {  // NOLINT
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
constexpr T sub_sat(T x, T y) noexcept {  // NOLINT
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
/// Multiply two integers, with saturation in case of overflow.
template <std::integral T>
constexpr T mul_sat(T x, T y) noexcept {  // NOLINT
    T z;
    if (!builtin_mul_overflow(x, y, &z)) {
        return z;
    }
    if constexpr (std::is_unsigned_v<T>) {
        return std::numeric_limits<T>::max();
    } else if (x < 0 != y < 0) {
        return std::numeric_limits<T>::min();
    } else {
        return std::numeric_limits<T>::max();
    }
}

template <std::integral T>
constexpr T div_sat(T x, T y) noexcept {  // NOLINT
    if constexpr (std::is_signed_v<T>) {
        if (x == std::numeric_limits<T>::min() && y == T(-1)) {
            return std::numeric_limits<T>::max();
        }
    }
    return x / y;
}

template <std::floating_point T>
constexpr T add_sat(T x, T y) noexcept {  // NOLINT
    return x + y;
}

template <std::floating_point T>
constexpr T sub_sat(T x, T y) noexcept {  // NOLINT
    return x - y;
}

template <std::floating_point T>
constexpr T mul_sat(T x, T y) noexcept {  // NOLINT
    return x * y;
}

template <std::floating_point T>
constexpr T div_sat(T x, T y) noexcept {  // NOLINT
    return x / y;
}

}  // namespace pdft::stdlike
