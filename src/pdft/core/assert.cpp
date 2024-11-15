#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include <cpptrace/cpptrace.hpp>

namespace pdft::core {

void fail(const char* message, std::source_location location) {
    GLOG(critical, message);
    GLOG(critical, "stack trace: {}", cpptrace::generate_trace().to_string(true));
    std::terminate();
}

}  // namespace pdft::core
