#pragma once

#include "pdft/tui/kitty/apc/command.h"
#include "platform/core/result.h"

namespace pdft::tui {

const std::error_category& parseResponseErrorCategory();

platform::Result<Response> parseResponse(std::string_view response);

}  // namespace pdft::tui
