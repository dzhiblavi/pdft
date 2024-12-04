#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/log.h"

namespace pdft::log {

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
    LoggerPtr logger_ = global();
};

}  // namespace pdft::log
