#pragma once

#include "pdft/core/result/type/error.h"

#include <expected>

namespace pdft {

template <typename T, typename E = Error>
using Result = std::expected<T, E>;

}  // namespace pdft
