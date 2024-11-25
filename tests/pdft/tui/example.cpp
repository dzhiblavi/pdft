#include "tests/pdft/run_coroutine.h"

#include "pdft/tui/terminal/ansi/terminal.h"

#include "pdft/image/image.h"
#include "pdft/tui/display/kitty/apc/tmux/executor.h"
#include "pdft/tui/display/kitty/unicode_display.h"

#include "pdft/tui/image/image.h"
#include "pdft/tui/terminal/ansi/terminal.h"
#include "pdft/tui/text/line.h"
#include "pdft/tui/text/span.h"
#include "pdft/tui/text/text.h"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

namespace pdft::tui {

TEST_CASE("tuiSpanExample", "") {
    core::io::OutBuffer buffer;
    terminal::ansi::Terminal terminal;

    auto span = text::Span::styled(
        "1111111111111111111",
        style::Patch{
            .foreground = style::ANSIColor::Red,
        });

    auto rect = layout::IRect{
        .x = 10,
        .y = 10,
        .width = 6,
        .height = 1,
    };

    auto ctx = Context{terminal, {}, buffer};
    text::render(span, rect, ctx);

    std::cout << buffer.view() << std::endl;
}

TEST_CASE("tuiLineExample", "") {
    core::io::OutBuffer buffer;
    terminal::ansi::Terminal terminal;

    auto line =
        text::Line::from(
            {
                text::Span::styled(
                    "1111",
                    style::Patch{
                        .foreground = style::ANSIColor::Red,
                    }),
                text::Span::styled(
                    "2222",
                    style::Patch{
                        .foreground = style::ANSIColor::Cyan,
                        .add_modifier = {style::Modifier::Italic},
                        .sub_modifier = {style::Modifier::Bold},
                    }),
            })
            .setStyle(style::Patch{.add_modifier = {style::Modifier::Bold}});

    auto rect = layout::IRect{
        .x = 10,
        .y = 20,
        .width = 7,
        .height = 1,
    };

    auto ctx = Context{terminal, {}, buffer};
    text::render(line, rect, ctx);

    std::cout << buffer.view() << std::endl;
}

TEST_CASE("tuiTextRawExample", "") {
    core::io::OutBuffer buffer;
    terminal::ansi::Terminal terminal;

    auto text = text::Text::raw("1111111111\n222222222222222\n333")
                    .setStyle(style::Patch{
                        .foreground = style::ANSIColor::Red,
                        .add_modifier = {style::Modifier::Italic},
                    });

    auto rect = layout::IRect{
        .x = 10,
        .y = 30,
        .width = 7,
        .height = 3,
    };

    auto ctx = Context{terminal, {}, buffer};
    text::render(text, rect, ctx);

    std::cout << buffer.view() << std::endl;
}

struct Image : pdft::image::Image {
    pdft::image::ImageSize size() const override {
        return {
            .width_px = 100,
            .height_px = 100,
        };
    }

    std::string_view samples() const override {
        static std::string image(3 * 100 * 100, char(255));
        return image;
    }
};

TEST_CASE("tuiImageExample", "") {
    tests::runCoroutine([](auto&&) -> asio::awaitable<void> {
        core::io::OutBuffer buffer;
        auto terminal = std::make_shared<terminal::ansi::Terminal>();

        Image the_image;
        display::kitty::UnicodeDisplay display(
            std::make_shared<display::kitty::apc::tmux::Executor>(
                display::kitty::apc::Config{}, nullptr),
            nullptr);
        co_await display.loadImage(13, the_image, buffer);

        auto image = image::Image::fromId(13);

        auto rect = layout::IRect{
            .x = 10,
            .y = 10,
            .width = 50,
            .height = 30,
        };

        auto ctx = Context{*terminal, {}, buffer};
        co_await render(image, rect, display, ctx);

        std::cout << buffer.view() << std::flush;
    });
}

}  // namespace pdft::tui
