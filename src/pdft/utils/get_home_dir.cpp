#include "pdft/utils/get_home_dir.h"

#include <cassert>

namespace pdft::utils {

std::filesystem::path getHomeDirectory() {
    auto* ptr = ::getenv("HOME");
    assert(ptr);
    return ptr;
}

std::filesystem::path expandHome(const std::filesystem::path& path) {
    std::string str = path;

    if (str.starts_with('~')) {
        str.replace(0, 1, pdft::utils::getHomeDirectory());
    }

    return str;
}

}  // namespace pdft::utils
