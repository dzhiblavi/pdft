#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/core/log/log.h"
#include "pdft/core/trace/detail/duration_suffix.h"
#include "pdft/core/trace/stack_scope_tracer.h"

namespace pdft::core::trace {

namespace detail {

template <typename Duration>
void logStackTrace(const StackScopeTracerBase& info, io::OutBuffer& out) {
    const auto& loc = info.loc;

    core::io::write(out, '\n');
    for (int d = info.depth; d > 0; d--) {
        core::io::write(out, '\t');
    }

    auto dur = std::chrono::duration_cast<Duration>(info.duration);
    core::io::write(
        out,
        std::format(
            "[{}] {}{} loc={}:{}",
            info.label,
            dur.count(),
            durationSuffix<Duration>,
            loc.file_name(),
            loc.line()));

    info.children.forEach([&](const auto& child) { logStackTrace<Duration>(child, out); });
}

}  // namespace detail

template <typename Duration = std::chrono::milliseconds>
inline auto stackTraceLogger(log::LoggerPtr logger = log::global()) {
    return [logger](const StackScopeTracerBase& info) {
        io::OutBuffer out;
        detail::logStackTrace<Duration>(info, out);
        logger->trace(out.view());
    };
}

}  // namespace pdft::core::trace
