#pragma once

#include "logging.h"

#include "pdft/app/controller/impl/controller.h"

#include <spdlog/common.h>

#include <cstdint>

namespace app {

struct SystemConfig {
    uint32_t max_event_queue_size = 10;
    bool enable_filesystem_watching = true;
};

struct DisplayConfig {
    float scaling_coeff = 1.f;
    bool invert_colors = false;
    bool force_unicode_kitty_display = false;
};

struct Config {
    std::optional<std::string> state_path = {};
    SystemConfig system = {};
    LoggingConfig logging = {};
    pdft::app::AppController::Config controller = {};
    DisplayConfig display = {};
};

}  // namespace app
