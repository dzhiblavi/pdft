#include "pdft/core/assert.h"

#include <cpptrace/cpptrace.hpp>
#include <iostream>

namespace pdft::core {

void fail(const char* message, std::source_location location) {
    std::cerr << std::format(
                     "FAIL\n{}\nat {}:{}\n{}",
                     message,
                     location.file_name(),
                     location.line(),
                     cpptrace::generate_trace().to_string(true))
              << std::endl;

    std::terminate();
}

}  // namespace pdft::core
