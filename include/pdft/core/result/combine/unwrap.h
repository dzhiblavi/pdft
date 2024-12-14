#pragma once

#include "pdft/core/result/type/result.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/common_adapters.h"  // IWYU pragma: keep

#include <optional>

namespace pdft::result {

namespace pipe {

struct Unwrap {
    template <typename T>
    T pipe(std::optional<T> optional) && {
        PDFT_VERIFY(optional.has_value(), "unwrap empty Maybe");
        return std::move(*optional);
    }

    template <typename T, typename E>
    T pipe(Result<T, E> result) && {
        PDFT_VERIFY(result.has_value(), "unwrap Result err: {}", result.error());
        return std::move(*result);
    }
};

}  // namespace pipe

inline static constexpr pipe::Unwrap unwrap;  // NOLINT

}  // namespace pdft::result
