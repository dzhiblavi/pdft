#pragma once

#include "pdft/core/assert.h"

namespace pdft::view {

enum class AxisFit {
    Center,
    Min,
    Max,
};

template <typename T>
requires std::integral<T> || std::floating_point<T>
T getOffset(T outer, T inner, view::AxisFit strategy) {
    PDFT_VERIFY(outer >= inner, "outer cannot be smaller than inner one");

    switch (strategy) {
        using enum view::AxisFit;

        case Min:
            return 0;
        case Max:
            return outer - inner;
        case Center:
            return (outer - inner) / static_cast<T>(2);

        default:
            core::fail("getOffset unexpected FitStrategy");
    }
}

}  // namespace pdft::view
