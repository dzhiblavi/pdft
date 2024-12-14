#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>

namespace pdft::utils {

template <size_t N>
struct FixedString {
    constexpr FixedString(const auto... _chars) : arr_{_chars..., '\0'} {}

    constexpr FixedString(const std::array<char, N> _arr) : arr_(_arr) {}

    constexpr FixedString(const char (&str)[N]) {
        std::copy_n(str, N, std::data(arr_));  // NOLINT
    }

    /// Returns the value as a string.
    std::string str() const {
        return std::string(string_view());
    }

    /// Returns the value as a string.
    constexpr std::string_view string_view() const {  // NOLINT
        return std::string_view(std::data(arr_), N - 1);
    }

    constexpr const char* c_str() const noexcept {  // NOLINT
        return arr_.data();
    }

    constexpr size_t size() const noexcept {
        return N - 1;
    }

    constexpr bool empty() const noexcept {
        return size() == 0;
    }

    std::array<char, N> arr_{};
};

template <size_t N>
FixedString(const char (&)[N]) -> FixedString<N>;

}  // namespace pdft::utils
