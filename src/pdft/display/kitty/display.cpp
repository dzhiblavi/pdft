#include "pdft/display/kitty/display.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"
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
    uint32_t img_rows;
    uint32_t img_columns;
    uint32_t img_top;
    uint32_t img_left;
};

uint32_t getOffset(uint32_t outer, uint32_t inner, FitStrategy strategy) {
    assert(outer >= inner);

    switch (strategy) {
        using enum FitStrategy;

        case Left:
            return 0;
        case Right:
            return outer - inner;
        case Center:
            return (outer - inner) / 2;

        default:
            core::fail("getOffset unexpected FitStrategy");
    }
}

KittyPosition fitPosition(float image_ratio, Position position) {
    const view::IRect& rect = position.rect;
    const float rect_ratio = float(rect.width) / rect.height;

    {
        auto [x, y, w, h] = rect;
        core::log::global()->info("rect: {}x{} @{},{}", w, h, x, y);
    }
    core::log::global()->info("img_ratio={}, rect_ratio={}", image_ratio, rect_ratio);

    const uint32_t in_img_rows =
        image_ratio >= rect_ratio ? floor(rect.width / image_ratio) : rect.height;

    const uint32_t in_img_cols =
        image_ratio >= rect_ratio ? rect.width : floor(rect.height * image_ratio);

    core::log::global()->info("in_img_rows={}, in_img_cols={}", in_img_rows, in_img_cols);

    const uint32_t out_img_rows =
        image_ratio >= rect_ratio ? rect.height : floor(rect.width / image_ratio);

    const uint32_t out_img_cols =
        image_ratio >= rect_ratio ? floor(rect.height * image_ratio) : rect.width;

    core::log::global()->info("out_img_rows={}, out_img_cols={}", out_img_rows, out_img_cols);

    switch (position.fit) {
        using enum FitStrategy;

        case Scale:
            return {
                .rows = rect.height,
                .columns = rect.width,
                .top = rect.y,
                .left = rect.x,
                .img_rows = rect.height,
                .img_columns = rect.width,
                .img_top = 0,
                .img_left = 0,
            };

        case ClipLeft:
            return {
                .rows = rect.height,
                .columns = rect.width,
                .top = rect.y,
                .left = rect.x,
                .img_rows = out_img_rows,
                .img_columns = out_img_cols,
                .img_top = 0,
                .img_left = 0,
            };

        case ClipRight:
            if (image_ratio >= rect_ratio) {
                return {
                    .rows = rect.height,
                    .columns = rect.width,
                    .top = rect.y,
                    .left = rect.x,
                    .img_rows = out_img_rows,
                    .img_columns = out_img_cols,
                    .img_top = 0,
                    .img_left = out_img_cols - rect.width,
                };
            } else {
                return {
                    .rows = rect.height,
                    .columns = rect.width,
                    .top = rect.y,
                    .left = rect.x,
                    .img_rows = out_img_rows,
                    .img_columns = out_img_cols,
                    .img_top = out_img_rows - rect.height,
                    .img_left = 0,
                };
            }

        case Center:
        case Left:
        case Right:
            if (image_ratio >= rect_ratio) {
                return {
                    .rows = in_img_rows,
                    .columns = rect.width,
                    .top = rect.y + getOffset(rect.height, in_img_rows, position.fit),
                    .left = rect.x,
                    .img_rows = in_img_rows,
                    .img_columns = in_img_cols,
                    .img_top = 0,
                    .img_left = 0,
                };
            } else {
                return {
                    .rows = rect.height,
                    .columns = in_img_cols,
                    .top = rect.y,
                    .left = rect.x + getOffset(rect.width, in_img_cols, position.fit),
                    .img_rows = in_img_rows,
                    .img_columns = in_img_cols,
                    .img_top = 0,
                    .img_left = 0,
                };
            }
    }
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

void Display::displayImage(ImageId id, Position position, core::OutBuffer& buffer) {
    auto term_size = core::terminal::getTerminalDimensions();
    auto image_size = [&]() -> const media::ImageSize& {
        auto it = loaded_.find(id);
        PDFT_VERIFY(it != loaded_.end());
        return it->second;
    }();

    float image_cols = float(image_size.width_px) / term_size.cell_width_px;
    float image_rows = float(image_size.height_px) / term_size.cell_height_px;
    float image_ratio = image_cols / image_rows;
    auto pos = fitPosition(image_ratio, position);

    {
        const auto& [r, c, t, l, ir, ic, it, il] = pos;
        core::log::global()->info(
            "pos: r={},c={},t={},l={}\tir={},ic={},it={},il={}", r, c, t, l, ir, ic, it, il);
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
        {"r", std::to_string(pos.img_rows)},
        {"c", std::to_string(pos.img_columns)},
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
                .rows = pos.top + row + 1,
                .columns = pos.left,
            },
            buffer);

        for (uint32_t col = 0; col < pos.columns; ++col) {
            core::write(
                buffer,
                ImagePlaceholder,
                RowColDiacritics[row + pos.img_top],
                RowColDiacritics[col + pos.img_left],
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
