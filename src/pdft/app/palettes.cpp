#include "pdft/app/palettes.h"

namespace pdft::app {

image::PaletteRef getSkimPalette() {
    using image::IRGBColor;

    static const auto palette = std::array{
        IRGBColor::gray(45),
        IRGBColor::gray(69),
        IRGBColor::gray(99),
        IRGBColor::gray(120),
        IRGBColor::gray(140),
        IRGBColor::gray(160),
        IRGBColor::gray(190),
        IRGBColor::gray(255),
    };

    return palette;
}

}  // namespace pdft::app
