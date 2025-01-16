#pragma once

#include "pdft/tui/kitty/kitty_display_base.h"
#include "pdft/tui/terminal.h"

#include "platform/module/module.h"

namespace pdft::tui {

class UnicodeKittyDisplay : public KittyDisplayBase, public platform::rt::Module {
 public:
    UnicodeKittyDisplay(
        Config config, platform::Arc<APCExecutor> executor, platform::Arc<const Terminal> terminal)
        : KittyDisplayBase(config, std::move(executor))
        , terminal_(std::move(terminal)) {}

    void displayImage(
        Context& ctx, ImageId id, const IRect& rect, const FRect& image_subarea, BufPair& buffers)
        override;

 private:
    platform::Arc<const Terminal> terminal_;
};

}  // namespace pdft::tui
