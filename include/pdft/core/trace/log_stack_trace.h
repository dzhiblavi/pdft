#pragma once

#include "pdft/core/trace/detail/duration_suffix.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/core/log.h"

#include <filesystem>

namespace pdft::trace {

struct AutomaticDuration {};

namespace detail {

std::string getAutomaticPrettyDuration(const SteadyDurationType& duration);

template <typename Duration>
std::string getPrettyDuration(const SteadyDurationType& duration) {
    auto dur = std::chrono::duration_cast<Duration>(duration);
    return std::format("{}{}", dur.count(), DurationSuffix<Duration>);
}

template <std::same_as<AutomaticDuration> Duration>
std::string getPrettyDuration(const SteadyDurationType& duration) {
    return getAutomaticPrettyDuration(duration);
}

template <typename Duration>
void logStackTrace(const StackScopeTracerBase& info, std::ostream& out) {
    const auto& loc = info.loc;

    out << '\n';
    for (auto d = info.depth; d > 0; d--) {
        out << "  ";
    }

    out << std::format(
        "[{}] {} loc={}:{}",
        info.label,
        getPrettyDuration<Duration>(info.duration),
        std::filesystem::path(loc.file_name()).filename().c_str(),
        loc.line());

    std::vector<const StackScopeTracerBase*> children;
    info.children.forEach([&](const auto& child) { children.push_back(&child); });
    std::reverse(children.begin(), children.end());

    for (const auto* child : children) {
        logStackTrace<Duration>(*child, out);
    }
}

}  // namespace detail

template <typename Duration = AutomaticDuration>
auto logStackTrace(
    const StackScopeTracerBase& info, log::LoggerPtr logger = log::loggerOrGlobal("trace")) {
    std::ostringstream os;
    detail::logStackTrace<Duration>(info, os);
    logger->trace(os.view());
}

template <typename Duration = AutomaticDuration>
auto logStackTrace(
    StackScopeTracerOwner owner, log::LoggerPtr logger = log::loggerOrGlobal("trace")) {
    owner.finalize();
    logStackTrace<Duration>(owner.getInfo(), logger);
}

}  // namespace pdft::trace
