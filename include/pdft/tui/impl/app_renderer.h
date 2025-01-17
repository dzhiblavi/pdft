#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/renderer.h"

#include "pdft/io/console_writer.h"
#include "pdft/io/termctl.h"

#include "platform/module/module.h"

namespace pdft::tui {

class AppRenderer : public Renderer, public platform::rt::Module {
 public:
    AppRenderer(
        platform::Arc<const tui::Terminal> terminal,
        platform::Arc<io::ConsoleWriter> console,
        platform::Arc<io::TermControl> term_control,
        platform::Arc<tui::Display> image_display);

    platform::Async<platform::Status> initScreen(platform::trace::Context& ctx) override;
    platform::Async<platform::Status> restoreScreen(platform::trace::Context& ctx) override;
    Screen newFrame(platform::trace::Context& ctx, tui::Element dom) override;
    platform::Async<platform::Status> renderFrame(platform::trace::Context& ctx) override;

 private:
    std::string tui_;
    io::OutBuffer control_;
    io::OutBuffer display_;

    platform::Arc<tui::Display> image_display_;
    platform::Arc<const tui::Terminal> terminal_;
    platform::Arc<io::TermControl> term_control_;
    platform::Arc<io::ConsoleWriter> console_;
};

}  // namespace pdft::tui
