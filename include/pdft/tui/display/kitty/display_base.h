#pragma once

#include "pdft/tui/display/display.h"
#include "pdft/tui/display/kitty/apc/executor.h"

#include <memory>
#include <unordered_map>

namespace pdft::tui::display::kitty {

class DisplayBase : public display::Display {
 public:
    DisplayBase(Arc<apc::Executor> executor) : executor_(std::move(executor)) {}

    void loadImage(
        Context& ctx,
        ImageId id,
        const pdft::image::Image& image,
        core::io::OutBuffer& buffer) override;

    void clear(Context& ctx, core::io::OutBuffer& buffer) override;
    void unload(Context& ctx, ImageId id, core::io::OutBuffer& buffer) override;
    void unloadAll(Context& ctx, core::io::OutBuffer& buffer) override;

    bool isLoaded(ImageId id) const override;

 protected:
    Arc<apc::Executor> executor_;
    std::unordered_map<ImageId, pdft::image::ImageSize> loaded_;
};

}  // namespace pdft::tui::display::kitty
