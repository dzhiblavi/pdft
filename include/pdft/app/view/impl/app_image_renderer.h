#pragma once

#include "pdft/app/state/palette_storage.h"
#include "pdft/app/view/image_renderer.h"

#include "pdft/runtime/module/module.h"

namespace pdft::app {

class AppImageRenderer : public ImageRenderer, public rt::Module {
 public:
    explicit AppImageRenderer(Arc<const PaletteStorage> palettes);

    void render(Context& ctx, const DisplayProperties& display, image::Image& image) const override;

 private:
    Arc<const PaletteStorage> palettes_;
};

}  // namespace pdft::app
