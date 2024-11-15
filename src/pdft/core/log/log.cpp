#include "pdft/core/log/log.h"
#include "pdft/core/assert.h"

#include <spdlog/spdlog.h>

namespace pdft::core::log {

namespace {

class GlobalLogger {
 public:
    static GlobalLogger& instance() {
        static GlobalLogger instance;
        return instance;
    }

    std::shared_ptr<spdlog::logger> get() {
        return logger;
    }

    void set(std::shared_ptr<spdlog::logger> instance) {
        logger = std::move(instance);
    }

 private:
    std::shared_ptr<spdlog::logger> logger;
};

}  // namespace

LoggerPtr getLogger(const std::string& name) {
    if (auto logger = spdlog::get(name)) {
        return logger;
    }

    return nullptr;
}

void setGlobalLogger(std::shared_ptr<spdlog::logger> logger) {
    GlobalLogger::instance().set(std::move(logger));
}

LoggerPtr getGlobalLogger() {
    PDFT_VERIFY(GlobalLogger::instance().get());
    return GlobalLogger::instance().get();
}

}  // namespace pdft::core::log
