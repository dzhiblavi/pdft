#pragma once

#include <cstdint>
#include <variant>

namespace pdft::tui::style {

enum class ANSIColor {
    Reset,
    // ANSI Color: Black. Foreground: 30, Background: 40
    Black,
    // ANSI Color: Red. Foreground: 31, Background: 41
    Red,
    // ANSI Color: Green. Foreground: 32, Background: 42
    Green,
    // ANSI Color: Yellow. Foreground: 33, Background: 43
    Yellow,
    // ANSI Color: Blue. Foreground: 34, Background: 44
    Blue,
    // ANSI Color: Magenta. Foreground: 35, Background: 45
    Magenta,
    // ANSI Color: Cyan. Foreground: 36, Background: 46
    Cyan,
    // ANSI Color: White. Foreground: 37, Background: 47
    // Note that this is sometimes called `silver` or `white` but we use `white` for bright white
    Gray,
    // ANSI Color: Bright Black. Foreground: 90, Background: 100
    // Note that this is sometimes called `light black` or `bright black` but we use `dark gray`
    DarkGray,
    // ANSI Color: Bright Red. Foreground: 91, Background: 101
    LightRed,
    // ANSI Color: Bright Green. Foreground: 92, Background: 102
    LightGreen,
    // ANSI Color: Bright Yellow. Foreground: 93, Background: 103
    LightYellow,
    // ANSI Color: Bright Blue. Foreground: 94, Background: 104
    LightBlue,
    // ANSI Color: Bright Magenta. Foreground: 95, Background: 105
    LightMagenta,
    // ANSI Color: Bright Cyan. Foreground: 96, Background: 106
    LightCyan,
    // ANSI Color: Bright White. Foreground: 97, Background: 107
    // Sometimes called `bright white` or `light white` in some terminals
    White,
};

struct RGBColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(const RGBColor&) const = default;
};

// An 8-bit 256 color.
// See also <https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit>
struct IndexedColor {
    uint8_t index;

    bool operator==(const IndexedColor&) const = default;
};

using Color = std::variant<ANSIColor, RGBColor, IndexedColor>;

}  // namespace pdft::tui::style
