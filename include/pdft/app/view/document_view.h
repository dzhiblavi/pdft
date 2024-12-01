#pragma once

#include "pdft/app/state/ui_state.h"
#include "pdft/app/view/page_renderer.h"

#include "pdft/tui/context.h"
#include "pdft/tui/elements.h"

namespace pdft::app::view {

class DocumentView {
 public:
    virtual ~DocumentView() = default;

    virtual tui::Element render(
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Context& ctx) = 0;
};

}  // namespace pdft::app::view
