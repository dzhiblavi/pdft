#include "pdft/tui/display/state.h"

#include "pdft/core/assert.h"

namespace pdft::tui::display {

bool State::isLoaded(image::ImageId id) const {
    return loaded_.contains(id);
}

image::ImageId State::addImage(image::ImageId id, LoadedImage loaded) {
    PDFT_VERIFY(!isLoaded(id));
    loaded_.emplace(id, loaded);
    return id;
}

void State::removeImage(image::ImageId id) {
    PDFT_VERIFY(isLoaded(id));
    loaded_.erase(id);
}

const State::LoadedImage& State::getImage(image::ImageId id) const {
    PDFT_VERIFY(isLoaded(id));
    return loaded_.find(id)->second;
}

}  // namespace pdft::tui::display
