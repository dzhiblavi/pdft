#pragma once

#include "pdft/core/result/type/result.h"

#include <span>

namespace pdft::cli {

// false = should exit
Result<bool> parse(
    std::span<char const*> argv, const std::string& description, const std::string& version);

}  // namespace pdft::cli
