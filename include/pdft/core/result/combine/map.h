#pragma once

#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/unit.h"

namespace pdft::result {

namespace pipe {

template <typename F>
struct [[nodiscard]] Map {
    F user;

    explicit Map(F u) : user(std::move(u)) {}

    // Non-copyable
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    template <typename T, typename E>
    requires(!std::same_as<T, Unit>)
    auto pipe(Result<T, E> r) && {
        return std::move(r).transform(std::move(user));
    }

    template <std::same_as<Unit> T, typename E>
    auto pipe(Result<T, E> r) && {
        return std::move(r).transform([&](Unit) { return std::move(user)(); });
    }

    template <typename T>
    auto pipe(Maybe<T> r) && {
        return std::move(r).transform(std::move(user));
    }
};

}  // namespace pipe

// Result<T> -> (T -> U) -> Result<U>
template <typename F>
auto map(F user) {
    return pipe::Map{std::move(user)};
}

}  // namespace pdft::result
