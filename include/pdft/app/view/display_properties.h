#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/tui/display.h"

#include "pdft/core/log/fmt_adapter.h"
#include "pdft/utils/hash_combine.h"

#include <optional>
#include <string>

namespace pdft::app {

struct Page {
    DocumentId document_id;
    uint32_t page_index;

    bool operator==(const Page&) const = default;
};

struct DisplayProperties {
    float scaling_coeff = 1.f;
    bool invert_colors = false;
    bool make_bolder = false;
    std::optional<std::string> palette = std::nullopt;

    bool operator==(const DisplayProperties&) const = default;
};

inline tui::ImageId getImageId(uint32_t salt, const Page& page, const DisplayProperties& display) {
    utils::hashCombine(
        salt,
        page.document_id,
        page.page_index,
        display.scaling_coeff,
        display.invert_colors,
        display.make_bolder,
        display.palette);

    return salt;
}

}  // namespace pdft::app

PDFT_LOG_ADAPT_TYPE_JSON(pdft::app, Page);
PDFT_LOG_ADAPT_TYPE_JSON(pdft::app, DisplayProperties);
