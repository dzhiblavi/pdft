#pragma once

#include <filesystem>

namespace pdft::notify {

struct Event {
    std::filesystem::path path;
};

}  // namespace pdft::notify
