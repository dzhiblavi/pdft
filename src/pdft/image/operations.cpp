#include "pdft/image/operations.h"

namespace pdft::image {

namespace {

constexpr Kernel<3> getBoldKernelInverted() {
    static constexpr Kernel<3> kernel{
        {
            {0.0f, 0.25f, 0.0f},
            {0.25f, 1.0f, 0.25f},
            {0.0f, 0.25f, 0.0f},
        },
    };
    return kernel;
}

constexpr Kernel<3> getBoldKernel() {
    static constexpr Kernel<3> kernel{
        {
            {0.0f, 0.25f, 0.0f},
            {0.25f, 0.0f, 0.25f},
            {0.0f, 0.25f, 0.0f},
        },
    };
    return kernel;
}

}  // namespace

void makeBolder(Image& image, bool inverted_colors) {
    if (inverted_colors) {
        applyKernel(image, getBoldKernelInverted());
    } else {
        applyKernel(image, getBoldKernel());
    }
}

}  // namespace pdft::image
