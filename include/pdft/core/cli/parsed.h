#pragma once

#include "pdft/core/cli/detail/parsed.h"

namespace pdft::cli {

template <typename C>
static Result<C> parsed() {
    return detail::parsed<C>(detail::getGlobalCliArgs<C>());
}

}  // namespace pdft::cli
