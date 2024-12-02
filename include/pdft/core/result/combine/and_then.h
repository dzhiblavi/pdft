#pragma once

#include "pdft/core/result/trait/value_of.h"
#include "pdft/core/result/type/result.h"

#include <type_traits>

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
    using U = result::trait::ValueOf<std::invoke_result_t<F, T>>;

    template <typename T>
    Result<U<T>> pipe(Result<T> r) && {
        return std::move(r).and_then(std::move(user));
    }

    template <typename T>
    Maybe<U<T>> pipe(Maybe<T> opt) && {
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
