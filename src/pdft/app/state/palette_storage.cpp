#include "pdft/app/state/palette_storage.h"

#include "pdft/core/module/register.h"
#include "pdft/core/result/make/none.h"
#include "pdft/core/result/make/some.h"

namespace pdft::app {

void PaletteStorage::add(const std::string& name, std::vector<image::RGBColor> palette) {
    PDFT_VERIFY(!palettes_.contains(name));
    palettes_[name] = std::move(palette);
}

void PaletteStorage::add(const std::string& name, image::PaletteRef palette) {
    add(name, std::vector<image::RGBColor>{palette.begin(), palette.end()});
}

Maybe<image::PaletteRef> PaletteStorage::get(const std::string& name) const {
    auto it = palettes_.find(name);

    if (it == palettes_.end()) {
        return result::none;
    }

    const auto& palette = it->second;
    return result::some(image::PaletteRef{palette.begin(), palette.end()});
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::PaletteStorage);
