#include "pdft/core/result.h"
#include "pdft/core/result/coro/maybe.h"   // IWYU pragma: keep
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep

#include <catch2/catch_test_macros.hpp>

namespace pdft {

using namespace result;

template <typename T>
using Res = Result<T, std::string>;

Res<int> just(int x) {
    co_return x;
}

Res<int> faulty(std::string err) {
    return result::makeError(err);
}

Maybe<int> getInt() {
    co_return 1;
}

Status aunit() {
    co_return unit;
}

TEST_CASE("coro/result/value/ok") {
    auto x = [] -> Res<int> {
        int x = co_await just(1);
        int y = co_await just(2);
        co_return x + y;
    }();

    CHECK(x.has_value());
    CHECK(*x == 3);
}

TEST_CASE("coro/result/value/err/last") {
    int count = 0;

    auto x = [&count] -> Res<int> {
        int x = co_await just(1);
        ++count;
        int y = co_await just(2);
        ++count;
        int z = co_await faulty("abcd");
        ++count;
        co_return x + y + z;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == "abcd");
    CHECK(count == 2);
}

TEST_CASE("coro/result/value/err/mid") {
    int count = 0;

    auto x = [&count] -> Res<int> {
        int x = co_await just(1);
        ++count;
        int y = co_await faulty("abcd");
        ++count;
        co_return x + y;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == "abcd");
    CHECK(count == 1);
}

TEST_CASE("coro/result/value/err/mapped") {
    int count = 0;

    auto x = [&count] -> Res<int> {
        int x = co_await just(1);
        ++count;
        int y =
            co_await (faulty("abcd") | result::mapErr([](auto) -> std::string { return "1234"; }));
        ++count;
        co_return x + y;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == "1234");
    CHECK(count == 1);
}

TEST_CASE("coro/result/value/err/lifted") {
    int z = 0;

    auto x = [&] -> Status {
        z = co_await (getInt() | result::lift(std::string("abcd")));
        co_await aunit();
        co_return unit;
    }();

    CHECK(x.has_value());
    CHECK(z == 1);
}

}  // namespace pdft
