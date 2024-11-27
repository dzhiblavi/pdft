#pragma once

#include "pdft/image/image.h"

#include <unordered_map>

namespace pdft::state {

class DisplayState {
 public:
    struct LoadedImage {
        // Dimensions of the loaded image
        float image_rows;
        float image_cols;

        // Other loaded image properties
        image::ImageProperties properties;
    };

    image::ImageId addImage(LoadedImage loaded, image::ImageId id);
    void removeImage(image::ImageId id);

    bool isLoaded(image::ImageId id) const;
    const LoadedImage& getImage(image::ImageId id) const;

 private:
    std::unordered_map<image::ImageId, LoadedImage> loaded_;
};

}  // namespace pdft::state
