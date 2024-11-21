#include "pdft/core/assert.h"

#include <cpptrace/cpptrace.hpp>
#include <iostream>

namespace pdft::core {

void fail(std::string_view message, std::source_location location) {
    std::cerr << "FAIL\n"
              << message << "\nat " << location.file_name() << ':' << location.line() << '\n'
              << cpptrace::generate_trace().to_string(true) << std::endl;

    std::terminate();
}

}  // namespace pdft::core
