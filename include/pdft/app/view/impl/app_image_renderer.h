#pragma once

#include "pdft/app/state/palette_storage.h"
#include "pdft/app/view/image_renderer.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

namespace pdft::app {

class AppImageRenderer : public ImageRenderer, public Module {
 public:
    explicit AppImageRenderer(inject<const PaletteStorage, "palettes"> palettes);

    void render(Context& ctx, const DisplayProperties& display, image::Image& image) const override;

 private:
    Arc<const PaletteStorage> palettes_;
};

}  // namespace pdft::app
