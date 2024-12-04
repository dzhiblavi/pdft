#include "pdft/image/operations.h"

#include <vector>

namespace pdft::image {

void invertColors(Image& image) {
    for (RGBColor& pixel : image.mutableSamples()) {
        pixel = RGBColor::gray(255) - pixel;
    }
}

void makeBolder(Image& image) {
    constexpr int positions[][2] = {
        {1, 0},
    };

    const auto size = image.size();
    const int width = size.width_px;
    const int height = size.height_px;
    auto samples = image.mutableSamples();

    for (int x = 0; x < width * height; ++x) {
        samples[x] = samples[x] + RGBColor::gray(std::size(positions) - 1);
        samples[x] = samples[x] / RGBColor::gray(std::size(positions));
    }

    std::vector<RGBColor> original{samples.begin(), samples.end()};

    for (auto&& [left, top] : positions) {
        for (int x = std::max(0, left); x < width + std::min(0, left); ++x) {
            for (int y = std::max(0, top); y < height + std::min(0, top); ++y) {
                int add_index = (x - left) + (y - top) * width;
                int index = x + y * width;
                samples[index] = samples[index] + original[add_index];
            }
        }
    }
}

}  // namespace pdft::image
