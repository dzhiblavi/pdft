#pragma once

#include <expected>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace app {

struct CliArgs {
    std::optional<std::string> config_path;
    std::vector<std::string> paths;
};

std::expected<CliArgs, std::string> parseCliArgs(std::span<const char*> args);

}  // namespace app
