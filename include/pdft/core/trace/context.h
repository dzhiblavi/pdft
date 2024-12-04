#pragma once

#include "pdft/core/context.h"
#include "pdft/utils/is_awaitable.h"

namespace pdft::trace {

template <std::invocable<> F>
requires(!utils::isAwaitable<std::invoke_result_t<F>>)
decltype(auto) trace(
    Context& ctx,
    const char* label,
    F&& func,
    std::source_location loc = std::source_location::current()) {
    auto _ = ctx.push(label, loc);
    return std::forward<F>(func)();
}

template <std::invocable<> F>
requires utils::isAwaitable<std::invoke_result_t<F>>
asio::awaitable<typename std::invoke_result_t<F>::value_type> trace(
    Context& ctx,
    const char* label,
    F&& func,
    std::source_location loc = std::source_location::current()) {
    auto _ = ctx.push(label, loc);
    co_return co_await std::forward<F>(func)();
}

template <typename T>
asio::awaitable<T> trace(
    Context& ctx,
    const char* label,
    asio::awaitable<T> value,
    std::source_location loc = std::source_location::current()) {
    auto _ = ctx.push(label, loc);
    co_return co_await std::move(value);
}

}  // namespace pdft::trace
