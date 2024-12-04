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

    template <typename T>
    requires(!std::same_as<T, Unit>)
    Result<T> pipe(Result<T> r) && {
        return std::move(r).transform_error(std::move(user));
    }

    template <std::same_as<Unit> T>
    Result<T> pipe(Result<T> r) && {
        return std::move(r).transform_error([&](Unit) { return std::move(user)(); });
    }
};

}  // namespace pipe

// Result<T> -> (E -> E) -> Result<T>
template <typename F>
auto mapErr(F user) {
    return pipe::MapErr{std::move(user)};
}

}  // namespace pdft::result
