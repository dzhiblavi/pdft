#pragma once

#include "pdft/core/result/type/status.h"

#include <span>

namespace pdft::cli {

Status parse(
    std::span<char const*> argv, const std::string& description, const std::string& version);

}  // namespace pdft::cli
