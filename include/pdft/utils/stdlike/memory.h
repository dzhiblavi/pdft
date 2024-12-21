#pragma once

#include <cstring>
#include <new>
#include <type_traits>

namespace pdft::stdlike {

template <class T>
requires(std::is_trivially_copyable_v<T> /* && std::is_implicit_lifetime_v<T>*/)
T* start_lifetime_as(void* p) noexcept {  // NOLINT
    return std::launder(static_cast<T*>(std::memmove(p, p, sizeof(T))));
}

template <class T>
requires(std::is_trivially_copyable_v<T> /* && std::is_implicit_lifetime_v<T>*/)
T* start_lifetime_as_array(void* p, size_t n) noexcept {  // NOLINT
    return std::launder(static_cast<T*>(std::memmove(p, p, sizeof(T) * n)));
}

}  // namespace pdft::cxxsup
