#pragma once

#include "pdft/core/result/type/maybe.h"

namespace pdft::result {

template <typename T>
Maybe<T> some(T x) {
    return {std::move(x)};
}

}  // namespace pdft::result
