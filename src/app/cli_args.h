#pragma once

#include "pdft/app/loader/impl/loader.h"
#include "pdft/utils/get_home_dir.h"

#include <expected>
#include <span>
#include <string>
#include <vector>

namespace app {

struct CliArgs {
    std::string config_path = pdft::utils::getHomeDirectory() / ".config/pdft/config.yaml";
    std::vector<std::string> paths;
    std::optional<pdft::app::StateMode> state_mode;
};

std::expected<CliArgs, std::string> parseCliArgs(std::span<const char*> args);

}  // namespace app
