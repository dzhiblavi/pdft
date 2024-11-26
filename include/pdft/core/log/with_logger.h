#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

namespace pdft::core::log {

class WithLogger {
 public:
    void setLogger(LoggerPtr logger) {
        logger_ = std::move(logger);
    }

    LoggerPtr getLogger() const {
        PDFT_VERIFY(logger_, "logger is not set");
        return logger_;
    }

 private:
    LoggerPtr logger_;
};

}  // namespace pdft::core::log
