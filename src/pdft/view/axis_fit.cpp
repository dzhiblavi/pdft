#include "pdft/view/axis_fit.h"
#include "pdft/core/assert.h"

namespace pdft::view {

uint32_t getOffset(uint32_t outer, uint32_t inner, view::AxisFit strategy) {
    PDFT_VERIFY(outer >= inner, "outer cannot be smaller than inner one");

    switch (strategy) {
        using enum view::AxisFit;

        case Min:
            return 0;
        case Max:
            return outer - inner;
        case Center:
            return (outer - inner) / 2;

        default:
            core::fail("getOffset unexpected FitStrategy");
    }
}

float getOffset(float outer, float inner, view::AxisFit strategy) {
    PDFT_VERIFY(outer >= inner, "outer cannot be smaller than inner one");

    switch (strategy) {
        using enum view::AxisFit;

        case Min:
            return 0;
        case Max:
            return outer - inner;
        case Center:
            return (outer - inner) / 2.f;

        default:
            core::fail("getOffset unexpected FitStrategy");
    }
}

}  // namespace pdft::view
