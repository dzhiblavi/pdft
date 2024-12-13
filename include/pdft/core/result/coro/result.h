#pragma once

#include "pdft/core/result/coro/return_object_holder.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"
#include "pdft/core/result/type/result.h"

#include "pdft/core/assert.h"

#include <coroutine>

namespace pdft::result {

template <typename T, typename E>
struct ResultPromiseBase {
    ResultPromiseBase() = default;

    auto get_return_object() noexcept {
        return ReturnedObjectHolder{&owner};
    }

    auto initial_suspend() noexcept {
        return std::suspend_never{};
    }

    auto final_suspend() noexcept {
        return std::suspend_never{};
    }

    [[noreturn]] void unhandled_exception() noexcept {
        panic("unhandled promise exception");
        std::unreachable();
    }

    ReturnedObjectHolder<Result<T, E>>* owner = nullptr;
};

template <typename T, typename E>
struct ResultPromise : ResultPromiseBase<T, E> {
    ResultPromise() = default;

    void return_value(T x) {
        PDFT_VERIFY_DBG(this->owner);
        this->owner->assign(ok(std::move(x)));
    }

    void return_value(E x) {
        PDFT_VERIFY_DBG(this->owner);
        this->owner->assign(err(std::move(x)));
    }
};

template <typename T, typename E>
struct ResultAwaitable {
    Result<T, E> object;

    bool await_ready() noexcept {
        return object.has_value();
    }

    T await_resume() {
        PDFT_VERIFY_DBG(object.has_value());
        return std::move(*object);
    }

    template <typename U>
    void await_suspend(std::coroutine_handle<ResultPromise<U, E>> h) {
        auto&& promise = h.promise();
        auto* owner = promise.owner;
        PDFT_VERIFY_DBG(owner);
        owner->assign(err(std::move(object.error())));
    }
};

}  // namespace pdft::result

template <typename T, typename E>
auto operator co_await(pdft::Result<T, E> o) {
    return pdft::result::ResultAwaitable<T, E>{std::move(o)};
}

namespace std {

template <typename T, typename E, typename... Args>
struct coroutine_traits<::pdft::Result<T, E>, Args...> {
    using promise_type = ::pdft::result::ResultPromise<T, E>;
};

}  // namespace std
