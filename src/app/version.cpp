#include "version.h"

#include "pdft/utils/build_info.h"

#include <magic_enum/magic_enum.hpp>

#include <format>

namespace app {

std::string getFullVersionString() {
    using namespace pdft::utils;
    using magic_enum::enum_name;

    auto [major, minor, patch] = getProjectVersion();

    return std::format(
        "{}.{}.{} ({}) commit {} built at {}",
        major,
        minor,
        patch,
        enum_name(getBuildType()),
        getGitCommitSHA1(),
        getBuildTimestamp());
}

}  // namespace app
