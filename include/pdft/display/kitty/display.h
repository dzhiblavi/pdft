#pragma once

#include "pdft/core/terminal/apc_executor.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/display/display.h"

#include <memory>
#include <unordered_map>

namespace pdft::display::kitty {

class Display : public display::Display {
 public:
    explicit Display(
        std::shared_ptr<core::terminal::Terminal> terminal,
        std::shared_ptr<core::terminal::APCExecutor> executor)
        : terminal_(std::move(terminal))
        , executor_(std::move(executor)) {}

    void loadImage(ImageId id, const media::Image& image, core::OutBuffer& buffer) override;
    void displayImage(ImageId id, view::Position position, core::OutBuffer& buffer) override;

    bool isLoaded(ImageId id) const override;
    void clear(core::OutBuffer& buffer) const override;

 private:
    void show(ImageId id, view::Position position, core::OutBuffer& buffer);

    std::shared_ptr<core::terminal::Terminal> terminal_;
    std::shared_ptr<core::terminal::APCExecutor> executor_;
    std::unordered_map<ImageId, media::ImageSize> loaded_;
};

}  // namespace pdft::display::kitty
