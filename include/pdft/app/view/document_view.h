#pragma once

#include "pdft/app/state/ui.h"

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"

#include "pdft/core/awaitable.h"
#include "pdft/core/context.h"

namespace pdft::app {

class DocumentView {
 public:
    virtual ~DocumentView() = default;

    virtual tui::Element getLayout(Context& ctx, const DocumentModel& doc) = 0;

    virtual Async<void> render(
        Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const = 0;
};

}  // namespace pdft::app
