#include "pdft/utils/get_home_dir.h"
#include "pdft/core/assert.h"

namespace pdft::utils {

std::filesystem::path getHomeDirectory() {
    return PDFT_VERIFY(::getenv("HOME"), "failed to get HOME environment variable");
}

std::filesystem::path expandHome(const std::filesystem::path& path) {
    std::string str = path;

    if (str.starts_with('~')) {
        str.replace(0, 1, pdft::utils::getHomeDirectory());
    }

    return str;
}

}  // namespace pdft::utils
