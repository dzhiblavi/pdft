#pragma once

#include <memory>

namespace pdft::core {

template <typename T>
using Box = std::unique_ptr<T>;

template <typename T>
using Arc = std::shared_ptr<T>;

}  // namespace pdft::core

namespace pdft {

using core::Arc;
using core::Box;

}  // namespace pdft
