#pragma once

#include "pdft/core/result/type/result.h"

namespace pdft::result {

namespace pipe {

template <typename F>
struct [[nodiscard]] Map {
    F user;

    explicit Map(F u) : user(std::move(u)) {}

    // Non-copyable
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    template <typename T>
    using U = std::invoke_result_t<F, T>;

    template <typename T>
    Result<U<T>> Pipe(Result<T> r) {
        return std::move(r).transform(user);
    }
};

}  // namespace pipe

template <typename F>
auto map(F user) {
    return pipe::Map{std::move(user)};
}

}  // namespace pdft::result
