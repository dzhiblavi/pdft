#pragma once

#include "pdft/core/module/detail/any_type.h"

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace pdft::module::detail {

namespace impl {

template <typename T, size_t N, typename... Head>
struct Constructor : std::false_type {};

template <typename T, typename... Head>
requires std::is_constructible_v<T, Head...>
struct Constructor<T, 0, Head...> : std::true_type {
    using type = std::tuple<Head...>;
};

template <typename T, size_t N>
requires(     //
    N > 0 &&  //
    Constructor<T, N - 1, AnyType1st<T>>::value)
struct Constructor<T, N> : std::true_type {
    using type = typename Constructor<T, N - 1, AnyType1st<T>>::type;
};

template <typename T, size_t N>
requires(
    N > 0 &&  //
    !Constructor<T, N - 1, AnyType1st<T>>::value && Constructor<T, N - 1, AnyType1stRef<T>>::value)
struct Constructor<T, N> : std::true_type {
    using type = typename Constructor<T, N - 1, AnyType1stRef<T>>::type;
};

template <typename T, size_t N, typename... Head>
requires(                   //
    N > 0 &&                //
    sizeof...(Head) > 0 &&  //
    Constructor<T, N - 1, Head..., AnyType>::value)
struct Constructor<T, N, Head...> : std::true_type {
    using type = typename Constructor<T, N - 1, Head..., AnyType>::type;
};

template <typename T, size_t N, typename... Head>
requires(
    N > 0 &&                //
    sizeof...(Head) > 0 &&  //
    !Constructor<T, N - 1, Head..., AnyType>::value &&
    Constructor<T, N - 1, Head..., AnyTypeRef>::value)
struct Constructor<T, N, Head...> : std::true_type {
    using type = typename Constructor<T, N - 1, Head..., AnyTypeRef>::type;
};

template <typename T, size_t N = 12>
consteval size_t maxNumConstructorArguments() {
    if constexpr (N == 0) {
        static_assert(
            std::is_constructible_v<T>, "Object is not constructible from under 12 arguments");
        return 0;
    } else if constexpr (Constructor<T, N>::value) {
        return N;
    } else {
        return maxNumConstructorArguments<T, N - 1>();
    }
}

}  // namespace impl

template <typename T>
static constexpr size_t MaxConstructorArguments = impl::maxNumConstructorArguments<T>();

template <typename T, size_t N = MaxConstructorArguments<T>>
using Constructor = impl::Constructor<T, N>;

}  // namespace pdft::module::detail
