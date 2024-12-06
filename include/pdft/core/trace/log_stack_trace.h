#pragma once

#include "pdft/core/trace/detail/duration_suffix.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/core/io/buffer.h"
#include "pdft/core/log.h"

#include <filesystem>

namespace pdft::trace {

struct AutomaticDuration {};

namespace detail {

std::string getAutomaticPrettyDuration(const DurationType& duration);

template <typename Duration>
std::string getPrettyDuration(const DurationType& duration) {
    auto dur = std::chrono::duration_cast<Duration>(duration);
    return std::format("{}{}", dur.count(), durationSuffix<Duration>);
}

template <std::same_as<AutomaticDuration> Duration>
std::string getPrettyDuration(const DurationType& duration) {
    return getAutomaticPrettyDuration(duration);
}

template <typename Duration>
void logStackTrace(const StackScopeTracerBase& info, io::OutBuffer& out) {
    const auto& loc = info.loc;

    io::write(out, '\n');
    for (int d = info.depth; d > 0; d--) {
        io::write(out, "  ");
    }

    io::write(
        out,
        std::format(
            "[{}] {} loc={}:{}",
            info.label,
            getPrettyDuration<Duration>(info.duration),
            std::filesystem::path(loc.file_name()).filename().c_str(),
            loc.line()));

    info.children.forEach([&](const auto& child) { logStackTrace<Duration>(child, out); });
}

}  // namespace detail

template <typename Duration = AutomaticDuration>
auto logStackTrace(
    const StackScopeTracerBase& info, log::LoggerPtr logger = log::loggerOrGlobal("trace")) {
    io::OutBuffer out;
    detail::logStackTrace<Duration>(info, out);
    logger->trace(out.view());
}

template <typename Duration = AutomaticDuration>
auto logStackTrace(
    StackScopeTracerOwner owner, log::LoggerPtr logger = log::loggerOrGlobal("trace")) {
    owner.finalize();
    logStackTrace<Duration>(owner.getInfo(), logger);
}

}  // namespace pdft::trace
