#pragma once

#include "pdft/image/image.h"

#include <array>

namespace pdft::image {

template <size_t N>
using Kernel = std::array<std::array<float, N>, N>;

void makeBolder(Image& image, bool inverted_colors);

template <ColorType T>
void invertColors(std::span<RGBColor<T>> pixels) {
    for (auto& pixel : pixels) {
        pixel = RGBColor<T>::gray(MaxColorValue<T>) - pixel;
    }
}

template <size_t N>
void applyKernel(Image& image, const Kernel<N>& kernel) {
    constexpr auto m = N / 2;

    auto [w, h] = image.size();
    auto pixels = image.mutableSamples();
    std::vector<IRGBColor> original{pixels.begin(), pixels.end()};

    for (uint32_t row = 0; row < h; ++row) {
        for (uint32_t col = 0; col < w; ++col) {
            auto result = FRGBColor::gray(0.f);

            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    if (row + i < m || row + i >= h + m) {
                        continue;
                    }
                    if (col + j < m || col + j >= w + m) {
                        continue;
                    }

                    auto ii = row + i - m;
                    auto jj = col + j - m;
                    float k = kernel[i][j];

                    if (k >= 0.f) {
                        result = result + floatColorFromInt(original[jj + ii * w]) * k;
                    } else {
                        result = result - floatColorFromInt(original[jj + ii * w]) * -k;
                    }
                }
            }

            pixels[col + row * w] = intColorFromFloat(result);
        }
    }
}

}  // namespace pdft::image
