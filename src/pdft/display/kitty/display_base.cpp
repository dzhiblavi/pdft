#include "pdft/display/kitty/display_base.h"

#include "pdft/core/assert.h"
#include "pdft/utils/base64.h"
#include "pdft/utils/zlib.h"

namespace pdft::display::kitty {

namespace {

std::string_view nextChunk(std::string_view& data, size_t max_chunk_size) {
    if (data.size() <= max_chunk_size) {
        auto result = data;
        data = "";
        return result;
    }

    auto result = data.substr(0, max_chunk_size);
    data = data.substr(max_chunk_size);
    return result;
}

}  // namespace

bool DisplayBase::isLoaded(media::ImageId id) const {
    return loaded_.contains(id);
}

asio::awaitable<void> DisplayBase::loadImage(
    media::ImageId id, const media::Image& image, core::io::OutBuffer& buffer) {
    PDFT_VERIFY(!isLoaded(id), "image with id={} is already loaded", id);
    auto image_size = image.size();
    loaded_.emplace(id, image_size);

    auto samples = image.samples();
    auto samples_compressed = utils::zlibCompress(samples);
    auto samples_compressed_b64 = utils::encodeBase64(samples_compressed);

    const size_t MaxChunkSize = 4096;

    std::string_view payload_left = samples_compressed_b64;
    auto first_chunk = nextChunk(payload_left, MaxChunkSize);

    {  // Execute the first request with the control data
        std::pair<std::string_view, std::string> control_data[] = {
            {"q", "2"},
            {"a", "t"},
            {"t", "d"},
            {"o", "z"},
            {"s", std::to_string(image_size.width_px)},
            {"v", std::to_string(image_size.height_px)},
            {"f", "24"},
            {"i", std::to_string(id)},
            {"m", payload_left.empty() ? "0" : "1"},
        };

        executor_->execute(
            {
                .command = "_G",
                .payload_b64 = first_chunk,
                .control_data = control_data,
            },
            buffer);
    }

    // Transmit the rest of the payload silently
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"m", "1"},
    };
    auto& [_, mv] = control_data[1];

    while (!payload_left.empty()) {
        auto chunk = nextChunk(payload_left, MaxChunkSize);

        if (payload_left.empty()) {
            mv = "0";
        }

        executor_->execute(
            {
                .command = "_G",
                .payload_b64 = chunk,
                .control_data = control_data,
            },
            buffer);
    }

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
            .payload_b64 = "",
            .control_data = control_data,
        },
        buffer);
}

}  // namespace pdft::display::kitty
