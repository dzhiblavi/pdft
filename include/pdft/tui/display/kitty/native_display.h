#pragma once

#include "pdft/tui/display/kitty/display_base.h"
#include "pdft/tui/terminal/terminal.h"

namespace pdft::tui::display::kitty {

class NativeDisplay : public DisplayBase {
 public:
    NativeDisplay(
        std::shared_ptr<apc::Executor> executor, std::shared_ptr<const terminal::Terminal> terminal)
        : DisplayBase(std::move(executor))
        , terminal_(std::move(terminal)) {}

    void displayImage(
        Context& ctx, ImageId id, const IRect& rect, const FRect& image_subarea, BufPair& buffers)
        override;

 private:
    std::shared_ptr<const terminal::Terminal> terminal_;
};

}  // namespace pdft::tui::display::kitty
