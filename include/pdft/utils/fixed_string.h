#pragma once

#include <algorithm>
#include <cstddef>

namespace pdft::utils {

template <size_t N>
struct FixedString {
    constexpr FixedString(const char (&s)[N + 1]) noexcept {
        std::copy_n(s, N + 1, name_);
    }

    constexpr const char* c_str() const noexcept {
        return name_;
    }

    constexpr size_t size() const noexcept {
        return N;
    }

    char name_[N + 1];
};

template <size_t N>
FixedString(const char (&)[N]) -> FixedString<N - 1>;

}  // namespace pdft::utils
