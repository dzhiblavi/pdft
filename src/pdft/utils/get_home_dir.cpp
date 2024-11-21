#include "pdft/utils/get_home_dir.h"
#include "pdft/core/assert.h"

namespace pdft::utils {

std::filesystem::path getHomeDirectory() {
    auto path = ::getenv("HOME");

    PDFT_VERIFY(
        path != nullptr, "failed to get HOME environment variable, received {}", (void*)path);

    return path;
}

}  // namespace pdft::utils
