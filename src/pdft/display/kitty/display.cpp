#include "pdft/display/kitty/display.h"
#include "pdft/core/assert.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/utils/base64.h"

#include "./detail/rowcolumn_diacritics.h"
#include "pdft/utils/zlib.h"

#include <zlib.h>

#include <cassert>

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

struct KittyPosition {
    uint32_t rows;
    uint32_t columns;
    uint32_t top;
    uint32_t left;
};

int getOffset(uint32_t outer, uint32_t inner, FitStrategy strategy) {
    assert(outer >= inner);

    switch (strategy) {
        using enum FitStrategy;

        case Scale:
            return 0;
        case Left:
            return 0;
        case Right:
            return outer - inner;
        case Center:
            return (outer - inner) / 2;
    }
}

KittyPosition fitPosition(uint32_t width, uint32_t height, Position position) {
    const auto& rect = position.rect;

    if (position.fit == FitStrategy::Scale) {
        return {
            .rows = rect.height,
            .columns = rect.width,
            .top = rect.y,
            .left = rect.x,
        };
    }

    float rect_ratio = float(rect.width) / rect.height;
    float ratio = float(width) / height;

    if (ratio >= rect_ratio) {
        uint32_t actual_height = floor(rect.width / ratio);
        return {
            .rows = actual_height,
            .columns = rect.width,
            .top = rect.y + getOffset(rect.height, actual_height, position.fit),
            .left = rect.x,
        };
    }

    uint32_t actual_width = floor(rect.height * ratio);
    return {
        .rows = rect.height,
        .columns = actual_width,
        .top = rect.y,
        .left = rect.x + getOffset(rect.width, actual_width, position.fit),
    };
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
        {"m", "_"},
    };

    while (!payload_left.empty()) {
        auto chunk = nextChunk(payload_left, MaxChunkSize);
        auto& [_, mv] = control_data[1];
        mv = payload_left.empty() ? "0" : "1";

        executor_->execute(
            {
                .command = "_G",
                .payload_b64 = chunk,
                .control_data = control_data,
            },
            buffer);
    }
}

void Display::displayImage(ImageId id, Position position, core::OutBuffer& buffer) {
    auto term_size = core::terminal::getTerminalDimensions();
    auto image_size = [&]() -> const media::ImageSize& {
        auto it = loaded_.find(id);
        PDFT_VERIFY(it != loaded_.end());
        return it->second;
    }();

    uint32_t image_cols = image_size.width_px / term_size.cell_width_px;
    uint32_t image_rows = image_size.height_px / term_size.cell_height_px;
    auto pos = fitPosition(image_cols, image_rows, position);

    std::pair<std::string_view, std::string> control_data[] = {
        {"q", "2"},
        {"a", "p"},
        {"U", "1"},
        {"i", std::to_string(id)},
        {"c", std::to_string(pos.columns)},
        {"r", std::to_string(pos.rows)},
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

    // Position cursor to the to left corner
    terminal_->moveCursor(
        core::terminal::CursorPosition{
            .rows = pos.top,
            .columns = pos.left,
        },
        buffer);

    // Fill the image space with special unicode characters
    const std::string_view ImagePlaceholder = "\U0010EEEE";
    const std::string_view IdChar = RowColDiacritics[(id >> 24) & 255];

    core::write(
        buffer, "\033[38:2:", (id >> 16) & 255, ":", (id >> 8) & 255, ":", (id >> 0) & 255, "m");

    for (uint32_t row = 0; row < pos.rows; ++row) {
        terminal_->moveCursor(
            core::terminal::CursorPosition{
                .rows = row + 1,
                .columns = pos.left,
            },
            buffer);

        for (uint32_t col = 0; col < pos.columns; ++col) {
            core::write(
                buffer, ImagePlaceholder, RowColDiacritics[row], RowColDiacritics[col], IdChar);
        }
    }

    core::write(buffer, "\033[39m");
}

}  // namespace pdft::display::kitty
