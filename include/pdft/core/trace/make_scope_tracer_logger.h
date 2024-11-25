#pragma once

#include "pdft/core/trace/detail/duration_suffix.h"

#include "pdft/core/log/log.h"
#include "pdft/core/trace/time_traits.h"
#include "pdft/core/trace/logger.h"
#include "pdft/utils/overloaded.h"

namespace pdft::core::trace {

template <typename Duration = std::chrono::milliseconds>
auto makeScopeTracerLogger(const char* label, log::LoggerPtr logger = trace::logger()) {
    return utils::Overloaded{
        [=](DurationType duration) {
            auto dur = std::chrono::duration_cast<Duration>(duration);
            logger->trace("[{}] {}{}", label, dur.count(), detail::durationSuffix<Duration>);
        },
    };
}

}  // namespace pdft::core::trace
