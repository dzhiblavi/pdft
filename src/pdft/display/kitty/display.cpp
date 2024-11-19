#include "pdft/display/kitty/display.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/display/fit.h"
#include "pdft/utils/base64.h"
#include "pdft/utils/zlib.h"

#include "./detail/rowcolumn_diacritics.h"

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

bool Display::isLoaded(ImageId id) const {
    return loaded_.contains(id);
}

void Display::loadImage(ImageId id, const media::Image& image, core::OutBuffer& buffer) {
    auto term_size = core::terminal::getTerminalDimensions();
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

void Display::displayImage(ImageId id, view::Position position, core::OutBuffer& buffer) {
    auto term_size = core::terminal::getTerminalDimensions();
    auto image_size = [&]() -> const media::ImageSize& {
        auto it = loaded_.find(id);
        PDFT_VERIFY(it != loaded_.end());
        return it->second;
    }();

    float image_cols = float(image_size.width_px) / term_size.cell_width_px;
    float image_rows = float(image_size.height_px) / term_size.cell_height_px;
    auto pos = fitPosition(image_cols, image_rows, position);

    {
        const auto& [r, c, t, l] = pos;
        core::log::global()->info("display @ r={},c={},t={},l={}", r, c, t, l);
    }

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
        {"r", std::to_string(pos.rows)},
        {"c", std::to_string(pos.columns)},
        {"C", "1"},
    };

    // Create a virtual placement
    executor_->execute(
        {
            .command = "_G",
            .payload_b64 = "",
            .control_data = control_data,
        },
        buffer);

    uint32_t left = static_cast<uint32_t>(std::max(0, pos.left));
    uint32_t top = static_cast<uint32_t>(std::max(0, pos.top));

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

    core::write(
        buffer, "\033[38:2:", (id >> 16) & 255, ":", (id >> 8) & 255, ":", (id >> 0) & 255, "m");

    for (uint32_t row = top; row < pos.top + pos.rows; ++row) {
        terminal_->moveCursor(
            core::terminal::CursorPosition{
                .rows = row + 1,
                .columns = left,
            },
            buffer);

        for (uint32_t col = left; col < pos.left + pos.columns; ++col) {
            core::write(
                buffer,
                ImagePlaceholder,
                detail::RowColDiacritics[row - pos.top],
                detail::RowColDiacritics[col - pos.left],
                IdChar);
        }
    }

    core::write(buffer, "\033[39m");
}

void Display::clear(core::OutBuffer& buffer) const {
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
