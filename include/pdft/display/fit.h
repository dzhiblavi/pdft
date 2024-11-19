#pragma once

#include "pdft/display/position.h"
#include "pdft/view/position.h"

namespace pdft::display {

Position fitPosition(float width, float height, view::Position position);

}  // namespace pdft::display
