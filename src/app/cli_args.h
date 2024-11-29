#pragma once

#include "state.h"

#include "pdft/utils/get_home_dir.h"

#include <expected>
#include <span>
#include <string>
#include <vector>

namespace app {

struct CliArgs {
    std::string config_path = pdft::utils::getHomeDirectory() / ".config/pdft/config.yaml";
    std::vector<std::string> paths;
    StateMode state_mode = StateMode::AddOnly;
};

std::expected<CliArgs, std::string> parseCliArgs(std::span<const char*> args);

}  // namespace app
