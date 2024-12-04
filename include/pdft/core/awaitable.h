#pragma once

#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/unit.h"

#include <asio/as_tuple.hpp>
#include <asio/use_awaitable.hpp>

#include <type_traits>

namespace pdft {

inline constexpr auto asio_coro_as_tuple = asio::as_tuple(asio::use_awaitable);

template <typename T>
using AsyncResult = std::conditional_t<
    std::is_same_v<void, T>,
    asio::awaitable<Result<Unit>>,
    asio::awaitable<Result<T>>>;

using AsyncStatus = AsyncResult<void>;

}  // namespace pdft
