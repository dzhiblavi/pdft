#pragma once

#include "pdft/tui/context.h"
#include "pdft/tui/layout/rect.h"
#include "pdft/tui/style/style.h"
#include "pdft/tui/terminal/terminal.h"

#include <cppcoro/generator.hpp>

#include <string>

namespace pdft::tui::text {

struct Span {
    std::string content;
    style::Patch patch;

    Span& setContent(std::string content) {
        this->content = std::move(content);
        return *this;
    }

    Span& setStyle(style::Patch patch) {
        this->patch = patch;
        return *this;
    }

    size_t getWidth() const {
        return content.size();
    }

    static Span raw(std::string content) {
        return Span().setContent(std::string(content));
    }

    static Span styled(std::string content, style::Patch patch) {
        return Span().setContent(std::string(content)).setStyle(patch);
    }

 private:
    Span() = default;
};

inline void render(const Span& span, const layout::IRect& rect, Context& ctx) {
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
