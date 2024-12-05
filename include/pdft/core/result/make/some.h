#pragma once

#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/unit.h"

namespace pdft::result {

template <typename T>
Maybe<T> some(T x) {
    return {std::move(x)};
}

inline Maybe<Unit> some() {
    return some(unit);
}

}  // namespace pdft::result
