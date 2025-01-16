#pragma once

#include "platform/core/result.h"

#include <span>

namespace pdft::rt {

platform::Result<int> start(
    const std::string& description, const std::string& version, std::span<const char*> argv);

}  // namespace pdft::rt
