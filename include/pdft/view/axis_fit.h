#pragma once

#include <cstdint>

namespace pdft::view {

enum class AxisFit {
    Center,
    Min,
    Max,
};

uint32_t getOffset(uint32_t outer, uint32_t inner, view::AxisFit strategy);

}  // namespace pdft::view
