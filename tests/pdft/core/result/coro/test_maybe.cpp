#include "pdft/core/result.h"
#include "pdft/core/result/coro/maybe.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft {

using namespace result;

Maybe<int> just(int x) {
    co_return x;
}

Maybe<int> faulty() {
    return std::nullopt;
}

TEST_CASE("testAndThenMaybeNone") {
    auto x = Maybe<int>{} | andThen([](int) { return some(10); });
    CHECK(!x.has_value());
}

TEST_CASE("testAndThenMaybeSome") {
    auto x = some(10) | andThen([](int x) { return some(x * 2); });
    CHECK(x.has_value());
    CHECK(*x == 20);
}

TEST_CASE("testLiftNone") {
    auto err = std::make_error_code(std::errc::io_error);
    auto x = Maybe<int>{} | lift(err);
    CHECK(!x.has_value());
    CHECK(x.error() == err);
}

TEST_CASE("testLiftSome") {
    auto err = std::make_error_code(std::errc::io_error);
    auto x = some(10) | lift(err);
    CHECK(x.has_value());
    CHECK(*x == 10);
}

TEST_CASE("testMapMaybeNone") {
    auto x = Maybe<int>{} | map([](int) { return 10; });
    CHECK(!x.has_value());
}

TEST_CASE("testMapMaybeSome") {
    auto x = some(10) | map([](int x) { return x * 2; });
    CHECK(x.has_value());
    CHECK(*x == 20);
}

TEST_CASE("testOrElseMaybeNone") {
    auto x = Maybe<int>{} | orElse([] { return some(10); });
    CHECK(x.has_value());
    CHECK(*x == 10);
}

TEST_CASE("testOrElseMaybeSome") {
    auto x = some(20) | orElse([] { return some(10); });
    CHECK(x.has_value());
    CHECK(*x == 20);
}

TEST_CASE("testUnwrapSome") {
    int x = some(20) | unwrap;
    CHECK(x == 20);
}

TEST_CASE("testCoroMaybeOk") {
    auto x = [] -> Maybe<int> {
        int x = co_await just(10);
        int y = co_await just(20);
        co_return x + y;
    }();

    CHECK(x.has_value());
    CHECK(*x == 30);
}

TEST_CASE("testCoroMaybeFail") {
    bool reached = false;

    auto x = [&reached] -> Maybe<int> {
        int x = co_await just(10);
        int y = co_await just(20);
        reached = true;
        co_return x + y + co_await faulty();
    }();

    CHECK(!x.has_value());
    CHECK(reached);
}

TEST_CASE("testCoroMaybeNoAwaitOk") {
    auto x = [] -> Maybe<int> {
        Maybe<int> x = just(10);
        return x;
    }();

    CHECK(x.has_value());
    CHECK(*x == 10);
}

TEST_CASE("testCoroMaybeNoAwaitFaulty") {
    auto x = [] -> Maybe<int> {
        Maybe<int> x = faulty();
        return x.value_or(20);
    }();

    CHECK(x.has_value());
    CHECK(*x == 20);
}

}  // namespace pdft
