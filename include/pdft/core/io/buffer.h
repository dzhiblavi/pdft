#pragma once

#include <sstream>

namespace pdft::core::io {

using OutBuffer = std::ostringstream;

template <typename... Args>
void write(OutBuffer& buffer, const Args&... args) {
    ((buffer << args), ...);
}

inline void clear(OutBuffer& buffer) {
    buffer.str("");
    buffer.clear();
}

}  // namespace pdft::core::io
