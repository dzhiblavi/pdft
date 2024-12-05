#include "palettes.h"

namespace app {

using namespace pdft;

image::PaletteRef getSkimPalette() {
    using image::RGBColor;

    static const auto palette = std::array{
        RGBColor::gray(45),
        RGBColor::gray(69),
        RGBColor::gray(99),
        RGBColor::gray(120),
        RGBColor::gray(140),
        RGBColor::gray(160),
        RGBColor::gray(190),
        RGBColor::gray(255),
    };

    return palette;
}

}  // namespace app
