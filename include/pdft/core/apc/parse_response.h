#pragma once

#include "pdft/core/apc/apc.h"
#include "pdft/core/result.h"

namespace pdft::core::apc {

Result<APCResponse> parseResponse(std::string_view response);

}  // namespace pdft::core::apc
