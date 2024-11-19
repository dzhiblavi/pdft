#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/view/position.h"

namespace pdft::view {

struct PagePlacement {
    // The position which specifies where and how the page will be displayed
    Position position;

    state::DocumentId document_id;
    uint32_t page_index;
};

struct TextPlacement {
    enum class OverflowMode {
        Truncate,  // Discard excessive text
        Wrap,      // Wrap excessive text to the next line(s)
    };

    // The position which specifies where and how the page will be displayed
    Position position;

    // What to do with text that overflows
    OverflowMode overflow;

    // The text to be displayed
    std::string_view text;
};

}  // namespace pdft::view
