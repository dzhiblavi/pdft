#include "pdft/tui/kitty/kitty_display_base.h"

#include "platform/core/assert.h"
#include "platform/log.h"

namespace pdft::tui {

using namespace platform;

KittyDisplayBase::KittyDisplayBase(Config config, platform::Arc<APCExecutor> executor)
    : executor_(std::move(executor))
    , loaded_(config.max_loaded_images) {}

bool KittyDisplayBase::isLoaded(ImageId id) const {
    return loaded_.contains(id);
}

void KittyDisplayBase::loadCompressedImage(
    platform::trace::Context& ctx,
    ImageId id,
    image::ImageSize size,
    std::string_view compressed_data,
    io::OutBuffer& buffer) {
    log::debug("loading a compressed image with id={} to the display", id);
    platform::verify(!isLoaded(id), "image with id={} is already loaded", id);

    if (auto unloaded = loaded_.add(id, size)) {
        auto&& [unloaded_id, _] = *unloaded;
        unload(ctx, unloaded_id, buffer);
    }

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "t"},
        {"t", "d"},
        {"s", std::to_string(size.width_px)},
        {"v", std::to_string(size.height_px)},
        {"f", "24"},
        {"i", std::to_string(id)},
    };

    executor_->execute(
        ctx,
        {
            .command = "_G",
            .payload = compressed_data,
            .control_data = control_data,
            .b64_encode = true,
            .compress = Compress::UseCompressed,
        },
        buffer);
}

void KittyDisplayBase::clear(platform::trace::Context& ctx, io::OutBuffer& buffer) {
    log::debug("clearing all placements from the display");

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "d"},  // Action = delete.
        {"d", "a"},  // Delete all visible placements. Do not free images' data.
    };

    executor_->execute(
        ctx,
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

void KittyDisplayBase::unload(platform::trace::Context& ctx, ImageId id, io::OutBuffer& buffer) {
    log::debug("unloading an image with id={} from the display", id);
    loaded_.remove(id);

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "d"},  // Action = delete.
        {"d", "I"},  // Delete an image specified by its id "i".
        {"i", std::to_string(id)},
    };

    executor_->execute(
        ctx,
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

void KittyDisplayBase::unloadAll(platform::trace::Context& ctx, io::OutBuffer& buffer) {
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
        ctx,
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);
}

}  // namespace pdft::tui
