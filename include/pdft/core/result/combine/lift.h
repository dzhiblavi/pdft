#pragma once

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/result.h"

namespace pdft::result {

namespace pipe {

template <typename E>
struct Lift {
    E error;

    template <typename T>
    Result<T, E> pipe(Maybe<T> opt) && {
        if (opt.has_value()) {
            return std::move(*opt);
        }

        return err(std::move(error));
    }
};

}  // namespace pipe

// Maybe<T> -> Result<T>
template <typename E>
auto lift(E err) {
    return pipe::Lift(std::move(err));
}

template <typename... Args>
auto lift(std::format_string<const Args&...> fmt, const Args&... args) {
    return pipe::Lift(std::format(fmt, args...));
}

}  // namespace pdft::result
