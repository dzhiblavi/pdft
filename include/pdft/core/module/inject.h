#pragma once

#include "pdft/core/module/detail/inject.h"

namespace pdft {

template <typename T, utils::FixedString ModuleName>
using inject = module::detail::Injected<T, ModuleName>;

}  // namespace pdft
