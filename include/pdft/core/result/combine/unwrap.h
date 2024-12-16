#pragma once

#include "pdft/core/result/type/result.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/common_adapters.h"  // IWYU pragma: keep

namespace pdft::result {

namespace pipe {

struct Unwrap {
    template <typename T>
    T pipe(std::optional<T> optional) && {
        if (!optional.has_value()) {
            return panic("unwrap empty maybe");
        }

        return std::move(*optional);
    }

    template <typename T, typename E>
    T pipe(Result<T, E> result) && {
        if (!result.has_value()) {
            return panic("unwrap {}", result.error());
        }

        return std::move(*result);
    }
};

}  // namespace pipe

inline static constexpr pipe::Unwrap unwrap;  // NOLINT

}  // namespace pdft::result
