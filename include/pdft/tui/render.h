#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"
#include "pdft/tui/terminal/terminal.h"

#include "pdft/core/io/buffer.h"
#include "pdft/core/io/console.h"
#include "pdft/core/trace/context.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include <ftxui/screen/screen.hpp>

#include <asio/awaitable.hpp>

namespace pdft::tui {

class Renderer {
 public:
    Renderer(
        std::shared_ptr<tui::terminal::Terminal> terminal,
        std::shared_ptr<core::io::Console> console)
        : terminal_(std::move(terminal))
        , console_(std::move(console)) {
        PDFT_VERIFY(terminal_);
        PDFT_VERIFY(console_);
    }

    template <std::invocable<core::Context&, Screen&> Renderer>
    asio::awaitable<void> renderOne(Renderer renderer) {
        using core::trace::trace;

        auto tracer = core::trace::getGlobalPool().getTracer("renderOne");
        auto ctx = core::Context();
        ctx.setTracer(tracer);
        auto screen = Screen{terminal_->getDimensions(), {control_, display_}};

        auto dom = trace(ctx, "renderUser", [&] { return renderer(ctx, screen); });
        auto tui = trace(ctx, "renderTUI", [&] {
            auto screen = ftxui::Screen::Create(ftxui::Dimension::Full());
            tui::Render(screen, dom);
            return screen.ToString();
        });

        terminal_->clear(control_);
        co_await trace(ctx, "renderFlush", console_->writeMany(control_, tui, display_));

        core::io::clear(control_);
        core::io::clear(display_);

        tracer.finalize();
        core::trace::logStackTrace(tracer.getInfo());
    }

 private:
    core::io::OutBuffer control_;
    core::io::OutBuffer display_;

    std::shared_ptr<tui::terminal::Terminal> terminal_;
    std::shared_ptr<core::io::Console> console_;
};

}  // namespace pdft::tui
