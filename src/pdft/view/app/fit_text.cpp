#include "pdft/view/app/fit_text.h"

namespace pdft::view::app {

namespace {

cppcoro::generator<std::string_view> lines(
    std::string_view text, uint32_t width, TextPlacement::OverflowMode mode) {
    size_t pos = 0;

    while (pos < text.size()) {
        size_t next_newline = text.find('\n', pos);
        size_t next_pos;
        uint32_t line_length;

        if (next_newline == std::string::npos) {
            line_length = text.size() - pos;
            next_pos = text.size();
        } else {
            line_length = next_newline - pos;
            next_pos = next_newline + 1;
        }

        if (line_length <= width) {
            co_yield text.substr(pos, line_length);
        } else {
            switch (mode) {
                using enum TextPlacement::OverflowMode;

                case Truncate:
                    co_yield text.substr(pos, width);
                    break;

                case Wrap:
                    while (line_length > 0) {
                        auto len = std::min(line_length, width);
                        line_length -= len;

                        co_yield text.substr(pos, len);
                        pos += len;
                    }
                    break;
            }
        }

        pos = next_pos;
    }
}

auto countLinesAndWidth(std::string_view text, uint32_t width, TextPlacement::OverflowMode mode) {
    uint32_t num_lines = 0;
    uint32_t max_width = 0;

    for (auto line : lines(text, width, mode)) {
        ++num_lines;
        max_width = std::max(max_width, uint32_t(line.size()));
    }

    return std::make_tuple(num_lines, max_width);
}

}  // namespace

cppcoro::generator<Line> fitText(
    const IRect& rect, const TextPlacement& placement, std::string_view text) {
    const auto& [hor, vert, overflow] = placement;

    auto [num_lines, max_width] = countLinesAndWidth(text, rect.width, overflow);
    num_lines = std::min(num_lines, rect.height);

    uint32_t x = rect.x + getOffset(rect.width, max_width, hor);
    uint32_t y = rect.y + getOffset(rect.height, num_lines, vert);
    uint32_t line_idx = 0;

    for (auto line : lines(text, rect.width, overflow)) {
        co_yield Line{
            .x = x + getOffset(max_width, line.size(), hor),
            .y = y + line_idx,
            .text = line,
        };

        if (++line_idx == num_lines) {
            break;
        }
    }
}

}  // namespace pdft::view::app
