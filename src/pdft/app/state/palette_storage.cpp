#include "pdft/app/state/palette_storage.h"

#include "pdft/runtime/module/register.h"

namespace pdft::app {

void PaletteStorage::add(const std::string& name, std::vector<image::IRGBColor> palette) {
    verify(!palettes_.contains(name));
    palettes_[name] = std::move(palette);
}

void PaletteStorage::add(const std::string& name, image::PaletteRef palette) {
    add(name, std::vector<image::IRGBColor>{palette.begin(), palette.end()});
}

Maybe<image::PaletteRef> PaletteStorage::get(const std::string& name) const {
    auto it = palettes_.find(name);

    if (it == palettes_.end()) {
        return none;
    }

    const auto& palette = it->second;
    return image::PaletteRef{palette.begin(), palette.end()};
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::PaletteStorage);
