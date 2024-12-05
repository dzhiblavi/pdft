#pragma once

#include "pdft/core/result/make/none.h"
#include "pdft/core/result/make/some.h"
#include "pdft/core/result/type/maybe.h"
#include "pdft/image/palette.h"

#include <unordered_map>

namespace pdft::app {

class PaletteStorage {
 public:
    PaletteStorage() = default;

    void add(const std::string& name, std::vector<image::RGBColor> palette) {
        PDFT_VERIFY(!palettes_.contains(name));
        palettes_[name] = std::move(palette);
    }

    void add(const std::string& name, image::PaletteRef palette) {
        add(name, std::vector<image::RGBColor>{palette.begin(), palette.end()});
    }

    Maybe<image::PaletteRef> get(const std::string& name) const {
        auto it = palettes_.find(name);

        if (it == palettes_.end()) {
            return result::none;
        }

        const auto& palette = it->second;
        return result::some(image::PaletteRef{palette.begin(), palette.end()});
    }

 private:
    std::unordered_map<std::string, std::vector<image::RGBColor>> palettes_;
};

}  // namespace pdft::app
