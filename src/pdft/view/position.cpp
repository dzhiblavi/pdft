#include "pdft/view/position.h"
#include "pdft/core/assert.h"

namespace pdft::view {

uint32_t getOffset(uint32_t outer, uint32_t inner, view::AxisFit strategy) {
    PDFT_VERIFY(outer >= inner);

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

}  // namespace pdft::view
