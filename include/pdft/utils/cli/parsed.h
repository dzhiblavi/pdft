#pragma once

#include "pdft/utils/cli/detail/parsed.h"

namespace pdft::cli {

template <typename C>
static Result<C> parsed() {
    return detail::parsed<C>();
}

}  // namespace pdft::cli
