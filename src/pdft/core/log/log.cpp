#include "pdft/core/log.h"

#include <spdlog/spdlog.h>

namespace pdft::log {

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

void setGlobalLogger(Arc<spdlog::logger> logger) {
    GlobalLogger::instance().set(std::move(logger));
}

namespace impl {

LoggerPtr global() {
    return GlobalLogger::instance().get();
}

}  // namespace impl

}  // namespace pdft::log
