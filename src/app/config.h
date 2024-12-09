#pragma once

#include "cli_args.h"
#include "logging.h"

#include "pdft/app/controller/impl/controller.h"
#include "pdft/utils/random.h"

#include <spdlog/common.h>

#include <cstdint>

namespace app {

struct SystemConfig {
    uint32_t max_event_queue_size = 10;
    bool enable_filesystem_watching = true;
    pdft::app::StateMode state_mode = pdft::app::StateMode::AddOnly;
    uint32_t render_pool_size = 4;
    uint32_t num_preload_backward = 2;
    uint32_t num_preload_forward = 4;
    uint32_t image_id_salt = pdft::utils::random<uint32_t>();
};

struct DisplayConfig {
    pdft::app::DisplayProperties properties = {};
    bool force_unicode_kitty_display = false;
};

struct Config {
    std::optional<std::string> state_path = {};
    SystemConfig system = {};
    LoggingConfig logging = {};
    pdft::app::AppController::Config controller = {};
    DisplayConfig display = {};
};

inline void applyCliArgsOverrides(const CliArgs& args, Config& config) {
    if (args.state_mode.has_value()) {
        config.system.state_mode = *args.state_mode;
    }
}

}  // namespace app
