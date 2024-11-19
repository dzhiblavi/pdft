#include "pdft/utils/get_home_dir.h"
#include "pdft/core/assert.h"

namespace pdft::utils {

std::filesystem::path getHomeDirectory() {
    auto path = ::getenv("HOME");
    PDFT_VERIFY(path);

    return path;
}

}  // namespace pdft::utils
