#pragma once

#include "pdft/app/state/ui_state.h"
#include "pdft/app/view/page_renderer.h"

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "pdft/core/context.h"

namespace pdft::app {

class DocumentView {
 public:
    virtual ~DocumentView() = default;

    virtual tui::Element render(
        Context& ctx,
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const = 0;
};

}  // namespace pdft::app
