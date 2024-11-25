#pragma once

#include "pdft/tui/layout/alignment.h"
#include "pdft/tui/text/span.h"
#include "pdft/tui/context.h"

#include <numeric>
#include <vector>

namespace pdft::tui::text {

struct Line {
    std::vector<Span> spans;
    style::Patch patch;
    std::optional<layout::Alignment> alignment;

    Line& setSpans(std::vector<Span> spans) {
        this->spans = std::move(spans);
        return *this;
    }

    Line& setStyle(style::Patch patch) {
        this->patch = patch;
        return *this;
    }

    Line& setAlignment(std::optional<layout::Alignment> alignment) {
        this->alignment = std::move(alignment);
        return *this;
    }

    size_t getWidth() const {
        return std::accumulate(spans.begin(), spans.end(), 0, [](size_t w, const auto& span) {
            return w + span.getWidth();
        });
    }

    static Line raw(std::string_view content) {
        return Line().setSpans({Span::raw(content)});
    }

    static Line styled(std::string_view content, style::Patch patch) {
        return raw(content).setStyle(patch);
    }

    static Line from(std::vector<Span> spans) {
        return Line().setSpans(std::move(spans));
    }

 private:
    Line() = default;
};

inline void render(const Line& line, layout::IRect rect, ContextRef ctx) {
    ctx.terminal.patchStyle(line.patch, ctx.buffer);
    layout::IRect span_rect = rect;
    size_t width = 0;

    if (line.alignment) {
        rect.x += layout::getOffset<size_t>(
            rect.width,
            std::min(line.getWidth(), static_cast<size_t>(rect.width)),
            *line.alignment);
    }

    for (const auto& span : line.spans) {
        span_rect.x = rect.x + width;
        span_rect.width = rect.width - width;
        render(span, span_rect, ctx);

        width += std::min(span.getWidth(), static_cast<size_t>(span_rect.width));
        if (width >= rect.width) {
            break;
        }
    }
}

}  // namespace pdft::tui::text
