#pragma once

#include "pdft/image/palette.h"

#include "platform/core/result.h"
#include "platform/module/module.h"

#include <unordered_map>

namespace pdft::app {

class PaletteStorage : public platform::rt::Module {
 public:
    PaletteStorage() = default;

    void add(const std::string& name, std::vector<image::IRGBColor> palette);
    void add(const std::string& name, image::PaletteRef palette);

    platform::Maybe<image::PaletteRef> get(const std::string& name) const;

 private:
    std::unordered_map<std::string, std::vector<image::IRGBColor>> palettes_;
};

}  // namespace pdft::app
