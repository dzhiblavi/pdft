#pragma once

#include "pdft/display/position.h"
#include "pdft/media/pdf/document.h"

namespace pdft::view {

struct Placement {
    display::Position position;
    media::pdf::DocumentId document_id;
    uint32_t page_index;
};

}  // namespace pdft::view
