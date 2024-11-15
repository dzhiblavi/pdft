#pragma once

#include <cassert>
#include <cstdint>

namespace pdft::utils {

inline uint32_t cantorPair(uint16_t a, uint16_t b) {
    uint32_t aa = a, bb = b;
    return (aa + bb) * (aa + bb + 1) / 2 + bb;
}

}  // namespace pdft::utils
