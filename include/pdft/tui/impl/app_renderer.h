#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/renderer.h"

#include "pdft/io/console_writer.h"
#include "pdft/io/termctl.h"

#include "pdft/core/module/module.h"

namespace pdft::tui {

class AppRenderer : public Renderer, public Module {
 public:
    AppRenderer(
        Arc<const tui::Terminal> terminal,
        Arc<io::ConsoleWriter> console,
        Arc<io::TermControl> term_control,
        Arc<tui::Display> image_display);

    Async<Status> initScreen(Context& ctx) override;
    Async<Status> restoreScreen(Context& ctx) override;
    Screen newFrame(Context& ctx, tui::Element dom) override;
    Async<Status> renderFrame(Context& ctx) override;

 private:
    std::string tui_;
    io::OutBuffer control_;
    io::OutBuffer display_;

    Arc<tui::Display> image_display_;
    Arc<const tui::Terminal> terminal_;
    Arc<io::TermControl> term_control_;
    Arc<io::ConsoleWriter> console_;
};

}  // namespace pdft::tui
