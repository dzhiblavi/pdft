#pragma once

#include "pdft/core/result/trait/value_of.h"
#include "pdft/core/result/type/result.h"

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include <optional>
#include <type_traits>

namespace pdft::result {

namespace pipe {

template <typename F>
struct [[nodiscard]] AndThen {
    F user;

    explicit AndThen(F u) : user(std::move(u)) {}

    // Non-copyable
    AndThen(const AndThen&) = delete;
    AndThen& operator=(const AndThen&) = delete;

    template <typename T>
    using U = result::trait::ValueOf<std::invoke_result_t<F, T>>;

    template <typename T>
    Result<U<T>> Pipe(Result<T> r) {
        return std::move(r).and_then(user);
    }

    template <typename T>
    Result<U<T>> Pipe(std::optional<T> opt) {
        if (opt.has_value()) {
            return user(std::move(*opt));
        }

        return err("empty optional");
    }
};

}  // namespace pipe

template <typename F>
auto andThen(F user) {
    return pipe::AndThen{std::move(user)};
}

}  // namespace pdft::result
