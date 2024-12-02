#pragma once

#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/status.h"

namespace pdft::result {

template <typename T>
Result<T> ok(T value) {
    return {std::move(value)};
}

inline Status ok() {
    return {unit};
}

}  // namespace pdft::result
