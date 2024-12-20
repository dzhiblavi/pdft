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
        if (!logger_) {
            panic("logger is not configured for this module");
        }

        return logger_;
    }

 private:
    LoggerPtr logger_ = global();
};

}  // namespace pdft::log
