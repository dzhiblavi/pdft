#pragma once

#include <string_view>

namespace pdft::utils {

struct SourceLocation {
    std::string_view file;
    int line;
    std::string_view function;
    std::string_view pretty_function;
};

}  // namespace pdft::utils

#define PDFT_SOURCE_LOCATION                                          \
    ::pdft::utils::SourceLocation {                                   \
        .file = __FILE__, .line = __LINE__, .function = __FUNCTION__, \
        .pretty_function = __PRETTY_FUNCTION__,                       \
    }
