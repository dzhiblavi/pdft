#include "pdft/tui/impl/app_renderer.h"

#include "platform/module/register.h"

namespace pdft::tui {

AppRenderer::AppRenderer(
    platform::Arc<const tui::Terminal> terminal,
    platform::Arc<io::ConsoleWriter> console,
    platform::Arc<io::TermControl> term_control,
    platform::Arc<tui::Display> image_display)
    : image_display_(std::move(image_display))
    , terminal_(std::move(terminal))
    , term_control_(std::move(term_control))
    , console_(std::move(console)) {}

platform::Async<platform::Status> AppRenderer::initScreen(platform::trace::Context& ctx) {
    io::clear(control_);
    term_control_->ioControl(io::ControlType::Input, {.canonical = false, .echo = false});
    terminal_->hideCursor(control_);
    terminal_->clear(control_);
    co_return co_await console_->write(control_);
}

platform::Async<platform::Status> AppRenderer::restoreScreen(platform::trace::Context& ctx) {
    io::clear(control_);
    terminal_->showCursor(control_);
    terminal_->clear(control_);
    image_display_->unloadAll(ctx, control_);
    co_return co_await console_->write(control_);
}

// 1. Initialize a new frame
Screen AppRenderer::newFrame(platform::trace::Context& ctx, tui::Element dom) {
    // Clear display buffers
    io::clear(control_);
    io::clear(display_);

    tui_ = ctx.push("render:tui") | [&] {
        auto screen = ftxui::Screen::Create(ftxui::Dimension::Full());
        tui::Render(screen, dom);
        return screen.ToString();
    }();

    // Clear images from the screen
    image_display_->clear(ctx, control_);
    return {terminal_->getDimensions(), control_, display_};
}

platform::Async<platform::Status> AppRenderer::renderFrame(platform::trace::Context& ctx) {
    terminal_->clear(control_);
    co_return ctx.push("render:flush") |
        co_await console_->writeMany(control_, std::move(tui_), display_);
}

}  // namespace pdft::tui

PLATFORM_REGISTER_MODULE(pdft::tui::AppRenderer);
