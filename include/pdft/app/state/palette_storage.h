#pragma once

#include "pdft/image/palette.h"

#include "pdft/runtime/module/module.h"
#include "pdft/core/result/type/maybe.h"

#include <unordered_map>

namespace pdft::app {

class PaletteStorage : public rt::Module {
 public:
    PaletteStorage() = default;

    void add(const std::string& name, std::vector<image::IRGBColor> palette);
    void add(const std::string& name, image::PaletteRef palette);

    Maybe<image::PaletteRef> get(const std::string& name) const;

 private:
    std::unordered_map<std::string, std::vector<image::IRGBColor>> palettes_;
};

}  // namespace pdft::app
