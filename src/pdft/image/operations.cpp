#include "pdft/image/operations.h"

#include <vector>

namespace pdft::image {

void invertColors(std::span<RGBColor> pixels) {
    static constexpr uint8_t max = 255;

    for (RGBColor& pixel : pixels) {
        pixel = RGBColor::gray(max) - pixel;
    }
}

void makeBolder(Image& image) {
    constexpr int positions[][2] = {
        {1, 0},
        {0, -1},
    };

    const auto size = image.size();
    const int width = int(size.width_px);
    const int height = int(size.height_px);
    auto samples = image.mutableSamples();

    // for (int x = 0; x < width * height; ++x) {
    // samples[x] = samples[x] + RGBColor::gray(std::size(positions) - 1);
    // samples[x] = samples[x] / RGBColor::gray(std::size(positions));
    //}

    std::vector<RGBColor> original{samples.begin(), samples.end()};

    for (auto&& [left, top] : positions) {
        for (int x = std::max(0, left); x < width + std::min(0, left); ++x) {
            for (auto y = std::max(0, top); y < height + std::min(0, top); ++y) {
                size_t add_index = (x - left) + (y - top) * width;
                size_t index = x + y * width;
                samples[index] = samples[index] + original[add_index];
            }
        }
    }
}

}  // namespace pdft::image
