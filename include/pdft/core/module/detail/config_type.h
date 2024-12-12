#pragma once

#include "pdft/core/result/type/unit.h"

namespace pdft::module::detail {

template <typename T>
concept HasConfig = requires() { typename T::Config; };

template <typename T>
concept HasCliArgs = requires() { typename T::CliArgs; };

template <typename T>
struct MaybeConfigTypeImpl {
    using type = Unit;
};

template <HasConfig T>
struct MaybeConfigTypeImpl<T> {
    using type = typename T::Config;
};

template <typename T>
using MaybeConfigType = typename MaybeConfigTypeImpl<T>::type;

template <typename T>
struct MaybeCliArgsTypeImpl {
    using type = Unit;
};

template <HasCliArgs T>
struct MaybeCliArgsTypeImpl<T> {
    using type = typename T::CliArgs;
};

template <typename T>
using MaybeCliArgsType = typename MaybeCliArgsTypeImpl<T>::type;

}  // namespace pdft::module::detail
