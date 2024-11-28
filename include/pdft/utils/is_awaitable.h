#pragma once

#include <asio/awaitable.hpp>

namespace pdft::utils {

template <typename T>
constexpr bool isAwaitable = false;

template <typename T>
constexpr bool isAwaitable<asio::awaitable<T>> = true;


}  // namespace pdft::utils
