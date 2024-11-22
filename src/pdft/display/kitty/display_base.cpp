#include "pdft/display/kitty/display_base.h"

#include "pdft/core/assert.h"

namespace pdft::display::kitty {

bool DisplayBase::isLoaded(media::ImageId id) const {
    return loaded_.contains(id);
}

asio::awaitable<void> DisplayBase::loadImage(
    media::ImageId id, const media::Image& image, core::io::OutBuffer& buffer) {
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

void DisplayBase::clear(core::io::OutBuffer& buffer) const {
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "d"},
    };

    executor_->execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

}  // namespace pdft::display::kitty
