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
    // The position which specifies where and how the page will be displayed
    Position position;

    // The text to be displayed
    std::string_view text;
};

}  // namespace pdft::view
