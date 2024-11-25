#include "pdft/tui/terminal/ansi/terminal.h"

#include "pdft/utils/visit.h"

namespace pdft::tui::terminal::ansi {

namespace {

int ansiForeground(style::ANSIColor color) {
    switch (color) {
        using enum style::ANSIColor;

        case Reset:
            return 39;
        case Black:
            return 30;
        case Red:
            return 31;
        case Green:
            return 32;
        case Yellow:
            return 33;
        case Blue:
            return 34;
        case Magenta:
            return 35;
        case Cyan:
            return 36;
        case Gray:
            return 37;
        case DarkGray:
            return 90;
        case LightRed:
            return 91;
        case LightGreen:
            return 92;
        case LightYellow:
            return 93;
        case LightBlue:
            return 94;
        case LightMagenta:
            return 95;
        case LightCyan:
            return 96;
        case White:
            return 97;
            break;
    }
}

int ansiBackground(style::ANSIColor color) {
    return 10 + ansiForeground(color);
}

void applyForeground(style::ANSIColor color, core::io::OutBuffer& buffer) {
    core::io::write(buffer, ansiForeground(color));
}

void applyBackground(style::ANSIColor color, core::io::OutBuffer& buffer) {
    core::io::write(buffer, ansiBackground(color));
}

void applyForeground(const style::RGBColor& color, core::io::OutBuffer& buffer) {
    const auto& [r, g, b] = color;
    core::io::write(buffer, "38;2;", r, ';', g, ';', b);
}

void applyBackground(const style::RGBColor& color, core::io::OutBuffer& buffer) {
    const auto& [r, g, b] = color;
    core::io::write(buffer, "48;2;", r, ';', g, ';', b);
}

void applyForeground(const style::IndexedColor& color, core::io::OutBuffer& buffer) {
    core::io::write(buffer, "38;5;", color.index);
}

void applyBackground(const style::IndexedColor& color, core::io::OutBuffer& buffer) {
    core::io::write(buffer, "48;5;", color.index);
}

void applyForeground(const style::Color& color, core::io::OutBuffer& buffer) {
    core::io::write(buffer, "\x1b[");
    utils::visit(color, [&buffer](const auto& color) { applyForeground(color, buffer); });
    core::io::write(buffer, 'm');
}

void applyBackground(const style::Color& color, core::io::OutBuffer& buffer) {
    core::io::write(buffer, "\x1b[");
    utils::visit(color, [&buffer](const auto& color) { applyBackground(color, buffer); });
    core::io::write(buffer, 'm');
}

int ansiSetModifier(style::Modifier modifier) {
    switch (modifier) {
        using enum style::Modifier;

        case Bold:
            return 1;

        case Dim:
            return 2;

        case Italic:
            return 3;

        case Underlined:
            return 4;

        case SlowBlink:
            return 5;

        case RapidBlink:
            return 6;

        case Reversed:
            return 7;

        case Hidden:
            return 8;

        case CrossedOut:
            return 9;
    }
}

int ansiModifier(style::Modifier modifier, bool add) {
    if (add) {
        return ansiSetModifier(modifier);
    }

    if (modifier == style::Modifier::Bold) {
        return 22;
    }

    return 20 + ansiSetModifier(modifier);
}

void writeModifiers(style::Modifiers mods, bool add, core::io::OutBuffer& buffer) {
    for (style::Modifier mod : magic_enum::enum_values<style::Modifier>()) {
        if (!mods.test(mod)) {
            continue;
        }

        mods.reset(mod);

        core::io::write(buffer, ansiModifier(mod, add));

        if (!mods.none()) {
            core::io::write(buffer, ';');
        }
    }
}

void patchModifiers(style::Modifiers add, style::Modifiers sub, core::io::OutBuffer& buffer) {
    if (add.none() && sub.none()) {
        return;
    }

    core::io::write(buffer, "\x1b[");

    if (add.any()) {
        writeModifiers(add, true, buffer);
    }

    if (sub.any()) {
        if (add.any()) {
            core::io::write(buffer, ';');
        }

        writeModifiers(sub, false, buffer);
    }

    core::io::write(buffer, 'm');
}

}  // namespace

void Terminal::clear(core::io::OutBuffer& buffer) {
    core::io::write(buffer, "\x1b[2J\x1b[H");
}

void Terminal::moveCursor(CursorPosition position, core::io::OutBuffer& buffer) {
    core::io::write(buffer, "\x1b[", position.rows + 1, ';', position.columns + 1, "H");
}

void Terminal::patchStyle(style::Patch patch, core::io::OutBuffer& buffer) {
    style::Style old_style = current_style_;
    style::apply(current_style_, patch);

    if (old_style.foreground != current_style_.foreground) {
        applyForeground(*current_style_.foreground, buffer);
    }

    if (old_style.background != current_style_.background) {
        applyBackground(*current_style_.background, buffer);
    }

    auto should_add = patch.add_modifier & ~old_style.modifiers;
    auto should_sub = patch.sub_modifier & old_style.modifiers;
    patchModifiers(should_add, should_sub, buffer);
}

}  // namespace pdft::tui::terminal::ansi
