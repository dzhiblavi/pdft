#pragma once

#include "pdft/utils/cli/detail/parse.h"

namespace pdft::cli {

template <typename S>
Result<S, std::string> parseFromCliArgs(std::span<const char*> args) {
    return detail::parseFromCliArgs<S>(args);
}

}  // namespace pdft::cli
