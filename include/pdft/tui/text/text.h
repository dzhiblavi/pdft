#pragma once

#include "pdft/tui/context.h"
#include "pdft/tui/text/line.h"
#include "pdft/tui/text/split_lines.h"

namespace pdft::tui::text {

struct Text {
    std::vector<Line> lines;
    style::Patch patch;
    std::optional<layout::Alignment> alignment;

    Text& setLines(std::vector<Line> lines) {
        this->lines = std::move(lines);
        return *this;
    }

    Text& setStyle(style::Patch patch) {
        this->patch = patch;
        return *this;
    }

    Text& setAlignment(std::optional<layout::Alignment> alignment) {
        this->alignment = alignment;
        return *this;
    }

    size_t getWidth() const {
        return std::ranges::max(lines, {}, &Line::getWidth).getWidth();
    }

    static Text raw(std::string content) {
        Text text;
        for (auto line : splitLines(content)) {
            text.lines.push_back(Line::raw(std::string(line)));
        }
        return text;
    }

    static Text styled(std::string content, style::Patch patch) {
        return raw(std::move(content)).setStyle(patch);
    }

    static Text from(std::vector<Line> lines) {
        return Text().setLines(std::move(lines));
    }

 private:
    Text() = default;
};

inline void render(const Text& text, const layout::IRect& rect, Context& ctx) {
    ctx.terminal.patchStyle(text.patch, ctx.buffer);
    layout::IRect line_rect = rect;

    for (auto line : text.lines) {
        if (text.alignment && !line.alignment) {
            line.alignment = text.alignment;
        }

        render(line, line_rect, ctx);

        if (++line_rect.y >= rect.y + rect.height) {
            break;
        }

        core::io::write(ctx.buffer, '\n');
    }
}

}  // namespace pdft::tui::text
