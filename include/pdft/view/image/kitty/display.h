#pragma once

#include "pdft/core/terminal/apc_executor.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/view/image/display.h"

#include <memory>
#include <unordered_set>

namespace pdft::view::image::kitty {

class Display : public image::Display {
 public:
    explicit Display(
        std::shared_ptr<core::terminal::Terminal> terminal,
        std::shared_ptr<core::terminal::APCExecutor> executor)
        : terminal_(std::move(terminal))
        , executor_(std::move(executor)) {}

    void loadImage(ImageId id, const media::Image& image, core::io::OutBuffer& buffer) override;
    void displayImage(ImageId id, IRect rect, core::io::OutBuffer& buffer) override;

    bool isLoaded(ImageId id) const override;
    void clear(core::io::OutBuffer& buffer) const override;

 private:
    void show(ImageId id, IRect rect, core::io::OutBuffer& buffer);

    std::shared_ptr<core::terminal::Terminal> terminal_;
    std::shared_ptr<core::terminal::APCExecutor> executor_;
    std::unordered_set<ImageId> loaded_;
};

}  // namespace pdft::view::image::kitty
