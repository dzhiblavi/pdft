#pragma once

#include "pdft/tui/display/display.h"

#include <unordered_map>

namespace pdft::tui::display {

class State {
 public:
    struct LoadedImage {
        float image_rows;
        float image_cols;
        float factor;
        bool invert_colors;
    };

    ImageId addImage(ImageId id, LoadedImage loaded);
    void removeImage(ImageId id);

    bool isLoaded(ImageId id) const;
    const LoadedImage& getImage(ImageId id) const;

 private:
    std::unordered_map<ImageId, LoadedImage> loaded_;
};

}  // namespace pdft::tui::display
