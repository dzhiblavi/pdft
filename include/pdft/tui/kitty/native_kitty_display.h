#pragma once

#include "pdft/tui/kitty/kitty_display_base.h"
#include "pdft/tui/terminal.h"

#include "pdft/core/module/module.h"

namespace pdft::tui {

class NativeKittyDisplay : public KittyDisplayBase, public Module {
 public:
    NativeKittyDisplay(Config config, Arc<APCExecutor> executor, Arc<const Terminal> terminal)
        : KittyDisplayBase(config, std::move(executor))
        , terminal_(std::move(terminal)) {}

    void displayImage(
        Context& ctx, ImageId id, const IRect& rect, const FRect& image_subarea, BufPair& buffers)
        override;

 private:
    Arc<const Terminal> terminal_;
};

}  // namespace pdft::tui
