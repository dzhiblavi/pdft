#pragma once

#include "pdft/core/result.h"
#include "pdft/tui/display/kitty/apc/command.h"

namespace pdft::tui::display::kitty::apc {

enum class ParseResponseError {
    MalformedResponse,
};

const std::error_category& parseResponseErrorCategory();

Result<Response> parseResponse(std::string_view response);

}  // namespace pdft::tui::display::kitty::apc
