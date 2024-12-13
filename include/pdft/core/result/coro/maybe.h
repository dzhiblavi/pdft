#pragma once

#include "pdft/core/result/coro/return_object_holder.h"
#include "pdft/core/result/type/maybe.h"

#include "pdft/core/assert.h"

#include <coroutine>

namespace pdft::result {

template <typename T>
struct MaybePromise {
    MaybePromise() = default;

    auto get_return_object() noexcept {
        return detail::ReturnedObjectHolder{&owner};
    }

    auto initial_suspend() noexcept {
        return std::suspend_never{};
    }

    auto final_suspend() noexcept {
        return std::suspend_never{};
    }

    void return_value(T value) {
        PDFT_VERIFY_DBG(owner);
        owner->assign(Maybe<T>(std::move(value)));
    }

    [[noreturn]] void unhandled_exception() noexcept {
        std::unreachable();
    }

    detail::ReturnedObjectHolder<Maybe<T>>* owner = nullptr;
};

template <typename T>
struct MaybeAwaitable {
    Maybe<T> object;

    auto await_ready() noexcept {
        return object.has_value();
    }

    auto await_resume() {
        PDFT_VERIFY_DBG(object.has_value());
        return std::move(*object);
    }

    template <typename U>
    void await_suspend(std::coroutine_handle<MaybePromise<U>> h) {
        auto&& promise = h.promise();
        auto* owner = promise.owner;
        PDFT_VERIFY_DBG(owner);
        owner->assign(std::nullopt);
    }
};

}  // namespace pdft::result

template <typename T>
auto operator co_await(pdft::Maybe<T> o) {
    return pdft::result::MaybeAwaitable<T>{std::move(o)};
}

namespace std {

template <typename T, typename... Args>
struct coroutine_traits<::pdft::Maybe<T>, Args...> {
    using promise_type = ::pdft::result::MaybePromise<T>;
};

}  // namespace std
