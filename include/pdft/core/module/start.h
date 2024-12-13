#pragma once

#include "pdft/core/result/type/result.h"

#include <span>

namespace pdft::module {

Result<int> start(
    const std::string& description, const std::string& version, std::span<const char*> argv);

}  // namespace pdft::module
