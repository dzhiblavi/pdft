#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/log/fmt_adapter.h"

#include "pdft/utils/stdlike/numeric.h"

namespace pdft::image {

template <typename T>
concept ColorType = std::same_as<T, uint8_t> || std::same_as<T, float>;

template <ColorType T>
inline constexpr T MaxColorValue;

template <>
inline constexpr float MaxColorValue<float> = 1.f;

template <>
inline constexpr uint8_t MaxColorValue<uint8_t> = 255;

template <ColorType T>
struct RGBColor {
    T r = T(0);
    T g = T(0);
    T b = T(0);

    bool operator==(const RGBColor&) const = default;

    static RGBColor gray(T shade) {
        return {shade, shade, shade};
    }
};

using IRGBColor = RGBColor<uint8_t>;
using FRGBColor = RGBColor<float>;

inline IRGBColor intColorFromFloat(const FRGBColor& color) {
    return {
        static_cast<uint8_t>(MaxColorValue<uint8_t> * color.r),
        static_cast<uint8_t>(MaxColorValue<uint8_t> * color.g),
        static_cast<uint8_t>(MaxColorValue<uint8_t> * color.b),
    };
}

inline FRGBColor floatColorFromInt(const IRGBColor& color) {
    return {
        float(color.r) / MaxColorValue<uint8_t>,
        float(color.g) / MaxColorValue<uint8_t>,
        float(color.b) / MaxColorValue<uint8_t>,
    };
}

inline RGBColor<uint8_t> rgbFromHex(std::string_view hex) {
    verify(hex.size() == 7);
    uint32_t value = std::stoul(hex.substr(1).data(), nullptr, 16);  // NOLINT
    constexpr uint32_t mask = 0x0000ff;

    return {
        static_cast<uint8_t>((value >> 16) & mask),  // NOLINT
        static_cast<uint8_t>((value >> 8) & mask),   // NOLINT
        static_cast<uint8_t>((value >> 0) & mask),   // NOLINT
    };
}

template <typename T>
RGBColor<T> operator+(const RGBColor<T>& a, const RGBColor<T>& b) {
    auto add = [](T a, T b) { return std::min(MaxColorValue<T>, stdlike::add_sat(a, b)); };
    return {add(a.r, b.r), add(a.g, b.g), add(a.b, b.b)};
}

template <typename T>
RGBColor<T> operator-(const RGBColor<T>& a, const RGBColor<T>& b) {
    auto sub = [](T a, T b) { return std::max(T(0), stdlike::sub_sat(a, b)); };
    return {sub(a.r, b.r), sub(a.g, b.g), sub(a.b, b.b)};
}

template <typename T>
RGBColor<T> operator/(const RGBColor<T>& a, const RGBColor<T>& b) {
    auto div = [](T a, T b) { return std::min(MaxColorValue<T>, stdlike::div_sat(a, b)); };
    return {div(a.r, b.r), div(a.g, b.g), div(a.b, b.b)};
}

template <typename T>
RGBColor<T> operator/(const RGBColor<T>& a, T b) {
    auto div = [](T a, T b) { return std::min(MaxColorValue<T>, stdlike::div_sat(a, b)); };
    return {div(a.r, b), div(a.g, b), div(a.b, b)};
}

template <typename T>
RGBColor<T> operator*(const RGBColor<T>& a, const RGBColor<T>& b) {
    auto mul = [](T a, T b) { return std::min(MaxColorValue<T>, stdlike::mul_sat(a, b)); };
    return {mul(a.r, b.r), mul(a.g, b.g), mul(a.b, b.b)};
}

template <typename T>
RGBColor<T> operator*(const RGBColor<T>& a, T b) {
    auto mul = [](T a, T b) { return std::min(MaxColorValue<T>, stdlike::mul_sat(a, b)); };
    return {mul(a.r, b), mul(a.g, b), mul(a.b, b)};
}

}  // namespace pdft::image

PDFT_LOG_ADAPT_TEMPLATE_JSON(pdft::image, RGBColor, T);
