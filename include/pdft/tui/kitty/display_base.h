#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/kitty/apc/apc_executor.h"

#include <unordered_map>

namespace pdft::tui {

class KittyDisplayBase : public Display {
 public:
    KittyDisplayBase(Arc<APCExecutor> executor) : executor_(std::move(executor)) {}

    void loadImage(
        Context& ctx,
        ImageId id,
        const pdft::image::Image& image,
        io::OutBuffer& buffer) override;

    void clear(Context& ctx, io::OutBuffer& buffer) override;
    void unload(Context& ctx, ImageId id, io::OutBuffer& buffer) override;
    void unloadAll(Context& ctx, io::OutBuffer& buffer) override;
    bool isLoaded(ImageId id) const override;

 protected:
    Arc<APCExecutor> executor_;
    std::unordered_map<ImageId, pdft::image::ImageSize> loaded_;
};

}  // namespace pdft::tui
