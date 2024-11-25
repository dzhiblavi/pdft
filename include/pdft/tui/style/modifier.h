#pragma once

#include <cstdint>

#include <magic_enum/magic_enum_containers.hpp>

namespace pdft::tui::style {

enum class Modifier : uint32_t {
    Bold = 1 << 0,
    Dim = 1 << 1,
    Italic = 1 << 2,
    Underlined = 1 << 3,
    SlowBlink = 1 << 4,
    RapidBlink = 1 << 5,
    Reversed = 1 << 6,
    Hidden = 1 << 7,
    CrossedOut = 1 << 8,
    // Or any combination of non-zero items
};

using Modifiers = magic_enum::containers::bitset<Modifier>;

}  // namespace pdft::tui::style
