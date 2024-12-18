#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/renderer.h"

#include "pdft/io/console_writer.h"
#include "pdft/io/termctl.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

namespace pdft::tui {

class AppRenderer : public Renderer, public Module {
 public:
    AppRenderer(
        inject<const tui::Terminal, "terminal"> terminal,
        inject<io::ConsoleWriter, "writer"> console,
        inject<io::TermControl, "term_ctrl"> term_control,
        inject<tui::Display, "display"> image_display);

    AsyncStatus initScreen(Context& ctx) override;
    AsyncStatus restoreScreen(Context& ctx) override;
    Screen newFrame(Context& ctx, tui::Element dom) override;
    AsyncStatus renderFrame(Context& ctx) override;

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
