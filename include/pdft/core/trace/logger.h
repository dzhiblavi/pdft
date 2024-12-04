#pragma once

#include "pdft/core/log.h"

namespace pdft::trace {

inline log::LoggerPtr logger() {
    if (auto logger = log::logger("trace")) {
        return logger;
    }

    return log::global();
}

}  // namespace pdft::core::trace
