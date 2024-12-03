#pragma once

#include <asio/as_tuple.hpp>
#include <asio/use_awaitable.hpp>

#include <type_traits>

namespace pdft {

inline constexpr auto asio_coro = asio::as_tuple(asio::use_awaitable);

template <typename T>
using AsyncResult = std::conditional_t<
    std::is_same_v<void, T>,
    asio::awaitable<std::tuple<asio::error_code>>,
    asio::awaitable<std::tuple<asio::error_code, T>>>;

}  // namespace pdft
