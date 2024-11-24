#pragma once

#include "pdft/core/result.h"
#include "pdft/display/kitty/apc/command.h"

namespace pdft::display::kitty::apc {

Result<display::kitty::apc::Response> parseResponse(std::string_view response);

}  // namespace pdft::display::kitty::apc
