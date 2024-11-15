#pragma once

#include <sstream>

namespace pdft::core {

using OutBuffer = std::ostringstream;

template <typename... Args>
void write(OutBuffer& buffer, const Args&... args) {
    ((buffer << args), ...);
}

}  // namespace pdft::core
