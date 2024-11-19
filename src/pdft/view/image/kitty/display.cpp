#include "pdft/view/image/kitty/display.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/utils/base64.h"
#include "pdft/utils/zlib.h"

#include "./detail/rowcolumn_diacritics.h"

namespace pdft::view::image::kitty {

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

bool Display::isLoaded(ImageId id) const {
    return loaded_.contains(id);
}

void Display::loadImage(ImageId id, const media::Image& image, core::io::OutBuffer& buffer) {
    PDFT_VERIFY(!isLoaded(id));
    auto image_size = image.size();
    loaded_.insert(id);

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

    // Transmit the rest of the payload
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
}

void Display::displayImage(ImageId id, IRect rect, core::io::OutBuffer& buffer) {
    auto term_size = core::terminal::getTerminalDimensions();

    const auto& [x, y, w, h] = rect;
    core::log::global()->info("display @ x={},y={},w={},h={}", x, y, w, h);

    // x, y, w, h (pixels) control the subrectangle of the image (not in unicode mode btw).
    // In case of Unicode mode (U=1) the image subrectangle is controlled via placing
    // the correct row and column indices.
    //
    // c, r (columns and rows) control the rectangle at which the image will be displayed
    // The rectangle position in achieved via moving the cursor to the top-left corner
    // before displaying the image.
    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},
        {"U", "1"},
        {"i", std::to_string(id)},
        {"r", std::to_string(h)},
        {"c", std::to_string(w)},
        {"C", "1"},
    };

    // Create a virtual PagePlacement
    executor_->execute(
        {
            .command = "_G",
            .payload_b64 = "",
            .control_data = control_data,
        },
        buffer);

    uint32_t left = static_cast<uint32_t>(std::max(0, x));
    uint32_t top = static_cast<uint32_t>(std::max(0, y));

    // Position cursor to the to left corner
    terminal_->moveCursor(
        core::terminal::CursorPosition{
            .rows = top,
            .columns = left,
        },
        buffer);

    // Fill the image space with special unicode characters
    const std::string_view ImagePlaceholder = "\U0010EEEE";
    const std::string_view IdChar = detail::RowColDiacritics[(id >> 24) & 255];

    core::io::write(
        buffer, "\033[38:2:", (id >> 16) & 255, ":", (id >> 8) & 255, ":", (id >> 0) & 255, "m");

    for (uint32_t row = top; row < std::min(term_size.rows, y + h); ++row) {
        terminal_->moveCursor(
            core::terminal::CursorPosition{
                .rows = row + 1,
                .columns = left + 1,
            },
            buffer);

        for (uint32_t col = left; col < std::min(term_size.columns, x + w); ++col) {
            core::io::write(
                buffer,
                ImagePlaceholder,
                detail::RowColDiacritics[row - y],
                detail::RowColDiacritics[col - x],
                IdChar);
        }
    }

    core::io::write(buffer, "\033[39m");
}

void Display::clear(core::io::OutBuffer& buffer) const {
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

}  // namespace pdft::view::image::kitty
