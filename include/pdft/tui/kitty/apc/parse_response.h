#pragma once

#include "pdft/core/result.h"
#include "pdft/tui/kitty/apc/command.h"

namespace pdft::tui {

const std::error_category& parseResponseErrorCategory();

Result<Response> parseResponse(std::string_view response);

}  // namespace pdft::tui
