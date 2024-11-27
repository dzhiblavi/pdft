#pragma once

#include "pdft/state/ui_state.h"
#include "pdft/tui/context.h"
#include "pdft/tui/elements.h"

namespace pdft::view {

class DocumentView {
 public:
    virtual ~DocumentView() = default;
    virtual tui::Element render(
        const state::DocumentState& state,
        const image::ImageProperties& image,
        tui::Context& ctx) = 0;
};

}  // namespace pdft::view
