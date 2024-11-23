#include "pdft/display/kitty/display_base.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

namespace pdft::display::kitty {

bool DisplayBase::isLoaded(media::ImageId id) const {
    return loaded_.contains(id);
}

asio::awaitable<void> DisplayBase::loadImage(
    media::ImageId id, const media::Image& image, core::io::OutBuffer& buffer) {
    log::debug("loading an image with id={} to the display", id);
    PDFT_VERIFY(!isLoaded(id), "image with id={} is already loaded", id);

    auto image_size = image.size();
    loaded_.emplace(id, image_size);

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "t"},
        {"t", "d"},
        {"s", std::to_string(image_size.width_px)},
        {"v", std::to_string(image_size.height_px)},
        {"f", "24"},
        {"i", std::to_string(id)},
    };

    executor_->execute(
        {
            .command = "_G",
            .payload = image.samples(),
            .control_data = control_data,
            .b64_encode = true,
            .compress = true,
        },
        buffer);

    co_return;
}

void DisplayBase::clear(core::io::OutBuffer& buffer) {
    log::debug("clearing all placements from the display");

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "d"},  // Action = delete.
        {"d", "a"},  // Delete all visible placements. Do not free images' data.
    };

    executor_->execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

void DisplayBase::unload(media::ImageId id, core::io::OutBuffer& buffer) {
    log::debug("unloading an image with id={} from the display", id);

    auto it = loaded_.find(id);
    PDFT_VERIFY(it != loaded_.end(), "tried to unload an image with unknown id {}", id);
    loaded_.erase(it);

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "d"},  // Action = delete.
        {"d", "I"},  // Delete an image specified by its id "i".
        {"i", std::to_string(id)},
    };

    executor_->execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

void DisplayBase::unloadAll(core::io::OutBuffer& buffer) {
    log::debug("unloading all {} images from the display", loaded_.size());
    loaded_.clear();

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "d"},  // Action = delete.
        {"d", "R"},  // Delete images with ids in [x, y]. Uppercase = remove images' data.
        {"x", "0"},
        {"y", std::to_string(std::numeric_limits<uint32_t>::max())},
    };

    executor_->execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

}  // namespace pdft::display::kitty
