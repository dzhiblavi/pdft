#pragma once

#include "pdft/utils/instanceof_template.h"

#include <asio/awaitable.hpp>

namespace pdft::utils {

template <typename T>
constexpr bool isAwaitable = IsInstanceOfTemplate<T, asio::awaitable>;

}  // namespace pdft::utils
