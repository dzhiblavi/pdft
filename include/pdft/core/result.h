#pragma once

#include <expected>
#include <string>

namespace pdft::core {

template <typename T>
using Result = std::expected<T, std::string>;

}  // namespace pdft::core

namespace pdft {

using core::Result;

}  // namespace pdft
