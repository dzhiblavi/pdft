#pragma once

#include "pdft/core/module/detail/injected.h"

namespace pdft {

template <typename T, utils::FixedString ModuleName>
using inject = module::detail::Injected<T, ModuleName>;

}  // namespace pdft
