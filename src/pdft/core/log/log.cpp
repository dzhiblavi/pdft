#include "pdft/core/log/log.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/load.h"

#include <spdlog/spdlog.h>

namespace pdft::core::log {

namespace {

class GlobalLogger {
 public:
    static GlobalLogger& instance() {
        static GlobalLogger instance;
        return instance;
    }

    LoggerPtr get() {
        return logger;
    }

    void set(LoggerPtr instance) {
        logger = std::move(instance);
    }

 private:
    LoggerPtr logger;
};

}  // namespace

LoggerPtr logger(const std::string& name) {
    if (auto logger = spdlog::get(name)) {
        return logger;
    }

    return nullptr;
}

void setGlobalLogger(std::shared_ptr<spdlog::logger> logger) {
    GlobalLogger::instance().set(std::move(logger));
}

namespace impl {

LoggerPtr global() {
    return GlobalLogger::instance().get();
}

}  // namespace impl

}  // namespace pdft::core::log
