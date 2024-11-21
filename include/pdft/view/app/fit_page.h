#pragma once

#include "pdft/view/placement.h"
#include "pdft/view/rect.h"

namespace pdft::view::app {

IRect fitPage(float width, float height, const IRect& rect, const PagePlacement& position);

}  // namespace pdft::view::app
