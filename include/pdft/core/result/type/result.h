#pragma once

#include "pdft/core/result/type/error.h"

#include <expected>
#include <optional>

namespace pdft {

template <typename T>
using Result = std::expected<T, Error>;

template <typename T>
using Maybe = std::optional<T>;

}  // namespace pdft
