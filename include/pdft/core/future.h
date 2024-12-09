#pragma once

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

}  // namespace pdft
