#pragma once

#include "pdft/state/document_storage.h"

#include <unordered_map>

namespace pdft::state {

struct Page {
    state::DocumentId document_id;
    uint32_t page_index;
};

class DisplayState {
 public:
    struct LoadedImage {
        media::PageSize page_size;
        bool inverted_colors;
        float factor;
        float image_rows;
        float image_cols;
    };

    image::ImageId addImage(LoadedImage loaded, image::ImageId id);
    void removeImage(image::ImageId id);

    bool isLoaded(image::ImageId id) const;
    const LoadedImage& getImage(image::ImageId id) const;

 private:
    std::unordered_map<image::ImageId, LoadedImage> loaded_;
};

}  // namespace pdft::state
