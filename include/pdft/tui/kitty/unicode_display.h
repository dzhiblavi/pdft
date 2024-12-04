#pragma once

#include "pdft/tui/kitty/display_base.h"
#include "pdft/tui/terminal.h"

namespace pdft::tui {

class UnicodeKittyDisplay : public KittyDisplayBase {
 public:
    UnicodeKittyDisplay(Arc<APCExecutor> executor, Arc<const Terminal> terminal)
        : KittyDisplayBase(std::move(executor))
        , terminal_(std::move(terminal)) {}

    void displayImage(
        Context& ctx, ImageId id, const IRect& rect, const FRect& image_subarea, BufPair& buffers)
        override;

 private:
    Arc<const Terminal> terminal_;
};

}  // namespace pdft::tui
