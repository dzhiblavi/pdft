#pragma once

#include <memory>

namespace pdft {

template <typename T>
using Box = std::unique_ptr<T>;

template <typename T>
using Arc = std::shared_ptr<T>;

}  // namespace pdft
