#pragma once

#include "pdft/view/placement.h"
#include "pdft/view/rect.h"

#include <cppcoro/generator.hpp>

namespace pdft::view::app {

struct Line {
    uint32_t x;
    uint32_t y;
    std::string_view text;

    bool operator==(const Line&) const = default;
};

cppcoro::generator<Line> fitText(
    const IRect& rect, const TextPlacement& placement, std::string_view text);

}  // namespace pdft::view::app
