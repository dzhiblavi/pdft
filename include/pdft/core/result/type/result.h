#pragma once

#include "pdft/core/result/type/error.h"

#include <expected>

namespace pdft {

template <typename T>
using Result = std::expected<T, Error>;

}  // namespace pdft
