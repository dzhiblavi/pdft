#pragma once

#include "pdft/runtime/cli/detail/parsed.h"

namespace pdft::rt {

template <typename C>
static Result<C> fromParsedCli() {
    return detail::parsed<C>(detail::getGlobalCliArgs<C>());
}

}  // namespace pdft::rt
