#pragma once

#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/unit.h"

namespace pdft::result {

namespace pipe {

template <typename F>
struct [[nodiscard]] MapErr {
    F user;

    explicit MapErr(F u) : user(std::move(u)) {}

    // Non-copyable
    MapErr(const MapErr&) = delete;
    MapErr& operator=(const MapErr&) = delete;

    template <typename T, typename E>
    requires(!std::same_as<E, Unit>)
    auto pipe(Result<T, E> r) && {
        return std::move(r).transform_error(std::move(user));
    }

    template <typename T, std::same_as<Unit> E>
    auto pipe(Result<T, E> r) && {
        return std::move(r).transform_error([&](E) { return std::move(user)(); });
    }
};

}  // namespace pipe

// Result<T, E> -> (E -> G) -> Result<T, G>
template <typename F>
auto mapErr(F user) {
    return pipe::MapErr{std::move(user)};
}

}  // namespace pdft::result
