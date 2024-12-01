#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/log/fmt_adapter.h"

namespace pdft::image {

struct RGBColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(const RGBColor&) const = default;
};

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
            (uint32_t(c.r) << 4) + (uint32_t(c.g) << 2) + (uint32_t(c.b) << 0));
    }
};
