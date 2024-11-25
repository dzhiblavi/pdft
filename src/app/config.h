#pragma once

#include "logging.h"

#include "pdft/controller/app/controller.h"
#include "pdft/image/image.h"

#include <spdlog/common.h>

#include <cstdint>

namespace app {

struct SystemConfig {
    uint32_t max_event_queue_size = 10;
    bool enable_filesystem_watching = true;
};

struct Config {
    SystemConfig system = {};
    LoggingConfig logging = {};
    pdft::controller::app::Controller::Config controller = {};
    pdft::image::ImageProperties image = {};
};

}  // namespace app
