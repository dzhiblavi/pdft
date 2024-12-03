#pragma once

#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/result.h"

namespace pdft::result {

namespace pipe {

template <typename F>
struct [[nodiscard]] OrElse {
    F user;

    explicit OrElse(F u) : user(std::move(u)) {}

    // Non-copyable
    OrElse(const OrElse&) = delete;
    OrElse& operator=(const OrElse&) = delete;

    template <typename T>
    Result<T> pipe(Result<T> r) && {
        return std::move(r).or_else(std::move(user));
    }

    template <typename T>
    Maybe<T> pipe(Maybe<T> opt) && {
        return std::move(opt).or_else(std::move(user));
    }
};

}  // namespace pipe

// Result<T> -> (() -> Result<T>) -> Result<T>
template <typename F>
auto orElse(F user) {
    return pipe::OrElse{std::move(user)};
}

}  // namespace pdft::result
