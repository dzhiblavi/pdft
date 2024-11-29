#pragma once

#include <filesystem>

namespace pdft::utils {

std::filesystem::path getHomeDirectory();

std::filesystem::path expandHome(const std::filesystem::path& path);

}  // namespace pdft::utils
