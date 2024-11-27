#include "pdft/state/display_state.h"

#include "pdft/core/assert.h"

namespace pdft::state {

bool DisplayState::isLoaded(image::ImageId id) const {
    return loaded_.contains(id);
}

image::ImageId DisplayState::addImage(LoadedImage loaded, image::ImageId id) {
    PDFT_VERIFY(!isLoaded(id));
    loaded_.emplace(id, loaded);
    return id;
}

void DisplayState::removeImage(image::ImageId id) {
    PDFT_VERIFY(isLoaded(id));
    loaded_.erase(id);
}

const DisplayState::LoadedImage& DisplayState::getImage(image::ImageId id) const {
    PDFT_VERIFY(isLoaded(id));
    return loaded_.find(id)->second;
}

}  // namespace pdft::state
