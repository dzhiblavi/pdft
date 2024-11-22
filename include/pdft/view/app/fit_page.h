#pragma once

#include "pdft/view/placement.h"
#include "pdft/view/rect.h"

#include <tuple>

namespace pdft::view::app {

std::tuple<IRect, FRect> fitPage(float wh_ratio, const IRect& rect, const PagePlacement& position);

}  // namespace pdft::view::app
