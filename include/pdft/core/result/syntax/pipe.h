#pragma once

#include "pdft/core/result/type/result.h"

#include <optional>

template <typename T, typename C>
auto operator|(pdft::Result<T> r, C c) {
    return c.Pipe(std::move(r));
}

template <typename T, typename C>
auto operator|(std::optional<T> r, C c) {
    return c.Pipe(std::move(r));
}
