#pragma once

#include "pdft/core/result/type/result.h"

namespace pdft::result {

namespace pipe {

template <typename F>
struct [[nodiscard]] MapErr {
    F user;

    explicit MapErr(F u) : user(std::move(u)) {}

    // Non-copyable
    MapErr(const MapErr&) = delete;
    MapErr& operator=(const MapErr&) = delete;

    template <typename T>
    Result<T> pipe(Result<T> r) && {
        return std::move(r).transform_error(std::move(user));
    }
};

}  // namespace pipe

// Result<T> -> (E -> E) -> Result<T>
template <typename F>
auto mapErr(F user) {
    return pipe::MapErr{std::move(user)};
}

}  // namespace pdft::result
