#pragma once

#include "pdft/core/result/type/result.h"

#include <span>

namespace pdft::rt {

Result<int> start(
    const std::string& description, const std::string& version, std::span<const char*> argv);

}  // namespace pdft::rt
