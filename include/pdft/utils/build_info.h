#pragma once

#include <string_view>

namespace pdft::utils {

enum class BuildType {
    Debug,
    Release,
};

enum class Toggle {
    ON,
    OFF
};

enum class Platform {
    Unknown,
    Linux,
    MacOS,
    Windows,
    Posix,
};

struct ProjectVersion {
    int major;
    int minor;
    int patch;
};

std::string_view getBuildTimestamp();

std::string_view getGitCommitSHA1();

ProjectVersion getProjectVersion();

BuildType getBuildType();

Platform getCurrentPlatform();

Toggle isBuiltWithASAN();

Toggle isBuiltWithUBSAN();

Toggle isBuiltWithTSAN();

}  // namespace pdft::utils
