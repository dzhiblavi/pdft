#pragma once

#include "pdft/core/assert.h"

// IWYU pragma: begin_exports
#include <result/coro.h>
#include <result/pipe.h>
#include <result/result.h>
#include <result/union.h>

#include <result/combine/and_then.h>
#include <result/combine/lift.h>
#include <result/combine/map.h>
#include <result/combine/map_err.h>
#include <result/combine/or_else.h>
// IWYU pragma: end_exports

#include <format>
#include <system_error>

namespace pdft {

using result::Unit;

template <typename T>
using Result = result::Result<T, std::string>;

using Status = Result<Unit>;

inline std::string errorCodeToString(std::error_code ec) {
    return std::format("{}: {}", ec.category().name(), ec.category().message(ec.value()));
}

template <typename T>
using Maybe = std::optional<T>;

inline constexpr std::nullopt_t none = std::nullopt;  // NOLINT

}  // namespace pdft

namespace result {

namespace pipe {

struct Unwrap {
    template <typename T>
    T pipe(std::optional<T> optional) && {
        if (!optional.has_value()) {
            return ::pdft::panic("unwrap empty maybe");
        }

        return std::move(*optional);
    }

    template <result::SomeResult R>
    typename R::value_type pipe(R r) && {
        if (!r.hasValue()) {
            return ::pdft::panic("unwrap {}", r.template error<std::string>());
        }

        return std::move(r).value();
    }
};

}  // namespace pipe

inline static constexpr pipe::Unwrap unwrap;  // NOLINT

}  // namespace result
