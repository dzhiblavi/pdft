#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/view/position.h"

namespace pdft::view {

struct Placement {
    // The position which specifies where and how the page will be displayed
    Position position;

    state::DocumentId document_id;
    uint32_t page_index;
};

}  // namespace pdft::view
