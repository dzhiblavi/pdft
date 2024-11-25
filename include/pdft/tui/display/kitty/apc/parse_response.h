#pragma once

#include "pdft/core/result.h"
#include "pdft/tui/display/kitty/apc/command.h"

namespace pdft::tui::display::kitty::apc {

Result<Response> parseResponse(std::string_view response);

}  // namespace pdft::tui::display::kitty::apc
