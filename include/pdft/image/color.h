#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/log/fmt_adapter.h"
#include "pdft/utils/sat.h"

namespace pdft::image {

struct RGBColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(const RGBColor&) const = default;

    static RGBColor gray(uint8_t shade) {
        return {shade, shade, shade};
    }
};

inline RGBColor operator+(const RGBColor& a, const RGBColor& b) {
    return {utils::add_sat(a.r, b.r), utils::add_sat(a.g, b.g), utils::add_sat(a.b, b.b)};
}

inline RGBColor operator-(const RGBColor& a, const RGBColor& b) {
    return {utils::sub_sat(a.r, b.r), utils::sub_sat(a.g, b.g), utils::sub_sat(a.b, b.b)};
}

inline RGBColor operator/(const RGBColor& a, const RGBColor& b) {
    return {
        static_cast<uint8_t>(a.r / b.r),
        static_cast<uint8_t>(a.g / b.g),
        static_cast<uint8_t>(a.b / b.b),
    };
}

inline RGBColor rgbFromHex(std::string_view hex) {
    PDFT_VERIFY(hex.size() == 7);
    uint32_t value = std::stoul(hex.substr(1).data(), nullptr, 16);
    constexpr uint32_t mask = 0x0000ff;

    return {
        static_cast<uint8_t>((value >> 16) & mask),
        static_cast<uint8_t>((value >> 8) & mask),
        static_cast<uint8_t>((value >> 0) & mask),
    };
}

}  // namespace pdft::image

PDFT_LOG_ADAPT_TYPE_JSON(pdft::image, RGBColor);

template <>
struct std::hash<pdft::image::RGBColor> {
    std::size_t operator()(const pdft::image::RGBColor& c) const {
        return std::hash<uint32_t>{}(
            (uint32_t(c.r) << 16) + (uint32_t(c.g) << 8) + (uint32_t(c.b) << 0));
    }
};
