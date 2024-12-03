#pragma once

#include "pdft/core/result/coro/return_object_holder.h"
#include "pdft/core/result/type/result.h"

#include "pdft/core/assert.h"

#include <coroutine>

namespace pdft::result {

template <typename T>
struct ResultPromise {
    ResultPromise() = default;

    auto get_return_object() noexcept {
        return ReturnedObjectHolder{&owner};
    }

    auto initial_suspend() noexcept {
        return std::suspend_never{};
    }

    auto final_suspend() noexcept {
        return std::suspend_never{};
    }

    [[clang::noinline]] void return_value(T x) {
        PDFT_VERIFY_DBG(owner);
        owner->assign(ok(std::move(x)));
    }

    [[noreturn]] void unhandled_exception() noexcept {
        core::panic("unhandled promise exception");
        std::unreachable();
    }

    ReturnedObjectHolder<Result<T>>* owner = nullptr;
};

template <typename T>
struct ResultAwaitable {
    Result<T> object;

    bool await_ready() noexcept {
        return object.has_value();
    }

    T await_resume() {
        PDFT_VERIFY_DBG(object.has_value());
        return std::move(*object);
    }

    template <typename U>
    void await_suspend(std::coroutine_handle<ResultPromise<U>> h) {
        auto&& promise = h.promise();
        auto* owner = promise.owner;
        PDFT_VERIFY_DBG(owner);
        owner->assign(err(std::move(object.error())));
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
