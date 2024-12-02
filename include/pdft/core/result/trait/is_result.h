#pragma once

#include "pdft/core/result/type/result.h"

namespace pdft::result::trait {

namespace match {

template <typename T>
struct IsResult {
    static constexpr bool kValue = false;
};

template <typename T>
struct IsResult<Result<T>> {
    static constexpr bool kValue = true;
};

}  // namespace match

template <typename T>
static constexpr bool IsResult = match::IsResult<T>::kValue;  // NOLINT

}  // namespace pdft::result::trait
