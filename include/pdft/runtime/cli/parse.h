#pragma once

#include "pdft/core/result.h"

#include <span>

namespace pdft::rt {

// false = should exit
Result<bool> parseCliArgs(
    std::span<char const*> argv, const std::string& description, const std::string& version);

}  // namespace pdft::rt
