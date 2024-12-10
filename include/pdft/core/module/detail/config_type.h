#pragma once

#include "pdft/core/result/type/unit.h"

namespace pdft::module::detail {

template <typename T, typename = void>
struct MaybeConfigTypeImpl {
    using type = Unit;
};

template <typename T>
struct MaybeConfigTypeImpl<T, std::void_t<typename T::Config>> {
    using type = typename T::Config;
};

template <typename T>
using MaybeConfigType = typename MaybeConfigTypeImpl<T>::type;

}  // namespace pdft::detail
