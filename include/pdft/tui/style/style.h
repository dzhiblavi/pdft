#pragma once

#include "pdft/tui/style/color.h"
#include "pdft/tui/style/modifier.h"

#include <optional>

namespace pdft::tui::style {

struct Style {
    std::optional<Color> foreground = std::nullopt;
    std::optional<Color> background = std::nullopt;
    Modifiers modifiers = Modifiers();
};

struct Patch {
    std::optional<Color> foreground = std::nullopt;
    std::optional<Color> background = std::nullopt;
    Modifiers add_modifier = Modifiers();
    Modifiers sub_modifier = Modifiers();
};

inline void apply(Style& style, const Patch& patch) {
    if (patch.foreground) {
        style.foreground = patch.foreground;
    }
    if (patch.background) {
        style.background = patch.background;
    }

    style.modifiers = style.modifiers | patch.add_modifier;
    style.modifiers = style.modifiers & ~patch.sub_modifier;
}

}  // namespace pdft::tui::style
