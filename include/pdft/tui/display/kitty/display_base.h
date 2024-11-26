#pragma once

#include "pdft/tui/display/display.h"
#include "pdft/tui/display/kitty/apc/executor.h"

#include "pdft/core/io/console.h"

#include <memory>
#include <unordered_map>

namespace pdft::tui::display::kitty {

class DisplayBase : public display::Display {
 public:
    DisplayBase(std::shared_ptr<apc::Executor> executor, std::shared_ptr<core::io::Console> console)
        : executor_(std::move(executor))
        , console_(std::move(console)) {}

    void loadImage(
        image::ImageId id, const image::Image& image, core::io::OutBuffer& buffer) override;

    void clear(core::io::OutBuffer& buffer) override;

    void unload(image::ImageId id, core::io::OutBuffer& buffer) override;

    void unloadAll(core::io::OutBuffer& buffer) override;

    bool isLoaded(image::ImageId id) const override;

 protected:
    std::shared_ptr<apc::Executor> executor_;
    std::shared_ptr<core::io::Console> console_;
    std::unordered_map<image::ImageId, image::ImageSize> loaded_;
};

}  // namespace pdft::tui::display::kitty
