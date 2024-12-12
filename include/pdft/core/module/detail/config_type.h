#pragma once

#include "pdft/core/result/type/unit.h"

namespace pdft::module::detail {

template <typename T>
concept HasConfig = requires() { typename T::Config; };

template <typename T>
concept HasCliConfig = requires() { typename T::CliConfig; };

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
struct MaybeCliConfigTypeImpl {
    using type = Unit;
};

template <HasCliConfig T>
struct MaybeCliConfigTypeImpl<T> {
    using type = typename T::CliConfig;
};

template <typename T>
using MaybeCliConfigType = typename MaybeCliConfigTypeImpl<T>::type;

}  // namespace pdft::module::detail
