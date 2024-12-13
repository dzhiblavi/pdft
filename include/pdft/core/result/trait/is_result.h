#pragma once

#include "pdft/core/result/type/result.h"

namespace pdft::result::trait {

namespace match {

template <typename T>
struct IsResult {
    static constexpr bool value = false;
};

template <typename T>
struct IsResult<Result<T>> {
    static constexpr bool value = true;
};

}  // namespace match

template <typename T>
static constexpr bool IsResult = match::IsResult<T>::value;  // NOLINT

}  // namespace pdft::result::trait
