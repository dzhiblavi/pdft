#pragma once

#include "pdft/core/result/coro/return_object_holder.h"
#include "pdft/core/result/type/result.h"

#include "pdft/core/assert.h"

#include <coroutine>

namespace pdft::result {

template <typename T>
struct ResultPromise {
    ReturnedObjectHolder<Result<T>>* owner;

    auto get_return_object() noexcept {
        return ReturnedObjectHolder{&owner};
    }

    auto initial_suspend() noexcept {
        return std::suspend_never{};
    }

    auto final_suspend() noexcept {
        return std::suspend_never{};
    }

    void return_value(T x) {
        owner->mutObject().emplace(std::move(x));
    }

    [[noreturn]] void unhandled_exception() noexcept {
        core::panic("unhandled promise exception");
        std::unreachable();
    }
};

template <typename T>
struct ResultAwaitable {
    Result<T> object;

    auto await_ready() noexcept {
        return object.has_value();
    }

    auto await_resume() {
        return std::move(*object);
    }

    template <typename U>
    void await_suspend(std::coroutine_handle<ResultPromise<U>> h) {
        h.promise().owner->mutObject() = err(std::move(object.error()));
        h.destroy();
    }
};

}  // namespace pdft::result

template <typename T>
auto operator co_await(pdft::Result<T> o) {
    return pdft::result::ResultAwaitable<T>{std::move(o)};
}

namespace std {

template <typename T, typename... Args>
struct coroutine_traits<::pdft::Result<T>, Args...> {
    using promise_type = ::pdft::result::ResultPromise<T>;
};

}  // namespace std
