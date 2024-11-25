#pragma once

#include "pdft/tui/context.h"
#include "pdft/tui/layout/rect.h"
#include "pdft/tui/style/style.h"
#include "pdft/tui/terminal/terminal.h"

#include <cppcoro/generator.hpp>

#include <string_view>

namespace pdft::tui::text {

struct Span {
    std::string_view content;
    style::Patch patch;

    Span& setContent(std::string_view content) {
        this->content = content;
        return *this;
    }

    Span& setStyle(style::Patch patch) {
        this->patch = patch;
        return *this;
    }

    size_t getWidth() const {
        return content.size();
    }

    static Span raw(std::string_view content) {
        return Span().setContent(content);
    }

    static Span styled(std::string_view content, style::Patch patch) {
        return Span().setContent(content).setStyle(patch);
    }

 private:
    Span() = default;
};

inline void render(const Span& span, const layout::IRect& rect, ContextRef ctx) {
    ctx.terminal.patchStyle(span.patch, ctx.buffer);
    ctx.terminal.moveCursor(
        {
            .rows = rect.y,
            .columns = rect.x,
        },
        ctx.buffer);

    core::io::write(
        ctx.buffer,
        span.content.substr(0, std::min(span.content.size(), static_cast<size_t>(rect.width))));
}

}  // namespace pdft::tui::text
