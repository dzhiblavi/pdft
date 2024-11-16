#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include <cpptrace/cpptrace.hpp>

namespace pdft::core {

void fail(const char* message, std::source_location location) {
    log::global()->critical(
        "FAIL\n{}\nat {}:{}\n{}",
        message,
        location.file_name(),
        location.line(),
        cpptrace::generate_trace().to_string(true));

    log::global()->flush();
    std::terminate();
}

}  // namespace pdft::core
