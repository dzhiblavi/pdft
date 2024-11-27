#pragma once

#include "pdft/image/image.h"

#include <unordered_map>

namespace pdft::tui::display {

class State {
 public:
    struct LoadedImage {
        // Dimensions of the loaded image
        float image_rows;
        float image_cols;

        // Other loaded image properties
        pdft::image::ImageProperties properties;
    };

    pdft::image::ImageId addImage(pdft::image::ImageId id, LoadedImage loaded);
    void removeImage(pdft::image::ImageId id);

    bool isLoaded(pdft::image::ImageId id) const;
    const LoadedImage& getImage(pdft::image::ImageId id) const;

 private:
    std::unordered_map<pdft::image::ImageId, LoadedImage> loaded_;
};

}  // namespace pdft::tui::display
