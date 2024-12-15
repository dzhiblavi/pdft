#pragma once

#include "pdft/core/awaitable.h"

#include <saf.hpp>

namespace pdft {

template <typename T>
using Future = saf::cc_future<T>;

template <typename T>
using Promise = saf::cc_promise<T>;

template <typename T, typename Executor>
std::tuple<Future<T>, Promise<T>> makeContract(Executor via) {
    Promise<T> promise{std::move(via)};
    Future<T> future = promise.get_future();
    return std::make_tuple(std::move(future), std::move(promise));
}

template <typename T>
Async<std::tuple<Future<T>, Promise<T>>> makeContract() {
    co_return makeContract<T>(co_await asio::this_coro::executor);
}

}  // namespace pdft
