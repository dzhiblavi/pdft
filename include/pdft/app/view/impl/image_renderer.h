#pragma once

#include "pdft/app/state/palette_storage.h"
#include "pdft/app/view/display_properties.h"
#include "pdft/image/image.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

namespace pdft::app {

class ImageRenderer : public Module {
 public:
    explicit ImageRenderer(inject<const PaletteStorage, "palettes"> palettes);

    void renderImage(Context& ctx, const DisplayProperties& display, image::Image& image) const;

 private:
    Arc<const PaletteStorage> palettes_;
};

}  // namespace pdft::app
