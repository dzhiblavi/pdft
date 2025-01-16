#pragma once

#include "pdft/app/state/palette_storage.h"
#include "pdft/app/view/image_renderer.h"

#include "platform/module/module.h"

namespace pdft::app {

class AppImageRenderer : public ImageRenderer, public platform::rt::Module {
 public:
    explicit AppImageRenderer(platform::Arc<const PaletteStorage> palettes);

    void render(Context& ctx, const DisplayProperties& display, image::Image& image) const override;

 private:
    platform::Arc<const PaletteStorage> palettes_;
};

}  // namespace pdft::app
