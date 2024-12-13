#pragma once

#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/unit.h"

namespace pdft::result {

namespace pipe {

template <typename F>
struct [[nodiscard]] AndThen {
    F user;

    explicit AndThen(F u) : user(std::move(u)) {}

    // Non-copyable
    AndThen(const AndThen&) = delete;
    AndThen& operator=(const AndThen&) = delete;

    template <typename T>
    requires(!std::same_as<T, Unit>)
    auto pipe(Result<T> r) && {
        return std::move(r).and_then(std::move(user));
    }

    template <std::same_as<Unit> T>
    auto pipe(Result<T> r) && {
        return std::move(r).and_then([&](Unit) { return std::move(user)(); });
    }

    template <typename T>
    auto pipe(Maybe<T> opt) && {
        return std::move(opt).and_then(std::move(user));
    }
};

}  // namespace pipe

// Result<T> -> (T -> Result<U>) -> Result<U>
template <typename F>
auto andThen(F user) {
    return pipe::AndThen{std::move(user)};
}

}  // namespace pdft::result
