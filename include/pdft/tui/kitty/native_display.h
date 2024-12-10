#pragma once

#include "pdft/tui/kitty/display_base.h"
#include "pdft/tui/terminal.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

namespace pdft::tui {

class NativeKittyDisplay : public KittyDisplayBase, public Module {
 public:
    NativeKittyDisplay(
        inject<APCExecutor, "apc"> executor, inject<const Terminal, "terminal"> terminal)
        : KittyDisplayBase(std::move(executor))
        , terminal_(std::move(terminal)) {}

    void displayImage(
        Context& ctx, ImageId id, const IRect& rect, const FRect& image_subarea, BufPair& buffers)
        override;

 private:
    Arc<const Terminal> terminal_;
};

}  // namespace pdft::tui
