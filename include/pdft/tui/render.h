#pragma once

#include "pdft/tui/elements.h"
#include "pdft/tui/screen.h"
#include "pdft/tui/terminal.h"

#include "pdft/core/context.h"
#include "pdft/core/io/buffer.h"
#include "pdft/core/io/console.h"
#include "pdft/core/rc.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include <ftxui/screen/screen.hpp>

#include <asio/awaitable.hpp>

namespace pdft::tui {

class Renderer {
 public:
    Renderer(Arc<const tui::Terminal> terminal, Arc<io::Console> console)
        : terminal_(std::move(terminal))
        , console_(std::move(console)) {
        PDFT_VERIFY(terminal_);
        PDFT_VERIFY(console_);
    }

    template <std::invocable<Context&, Screen&> Renderer>
    asio::awaitable<void> renderOne(Renderer renderer) {
        auto tracer = trace::getGlobalPool().getTracer("render:one");
        auto ctx = Context();
        ctx.setTracer(tracer);
        auto screen = Screen{terminal_->getDimensions(), {control_, display_}};

        auto dom = ctx.push("render:user") | renderer(ctx, screen);
        auto tui = ctx.push("render:tui") | [&] {
            auto screen = ftxui::Screen::Create(ftxui::Dimension::Full());
            tui::Render(screen, dom);
            return screen.ToString();
        }();

        terminal_->clear(control_);
        ctx.push("render:flush") | co_await console_->writeMany(control_, tui, display_);

        io::clear(control_);
        io::clear(display_);

        trace::logStackTrace(std::move(tracer));
    }

 private:
    io::OutBuffer control_;
    io::OutBuffer display_;

    Arc<const tui::Terminal> terminal_;
    Arc<io::Console> console_;
};

}  // namespace pdft::tui
