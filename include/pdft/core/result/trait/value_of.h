#pragma once

#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/result.h"

namespace pdft::result::trait {

namespace match {

template <typename T>
struct ValueOf;

template <typename T>
struct ValueOf<Result<T>> {
    using Type = T;
};

template <typename T>
struct ValueOf<Maybe<T>> {
    using Type = T;
};

}  // namespace match

template <typename Result>
using ValueOf = typename match::ValueOf<Result>::Type;

}  // namespace pdft::result::trait
