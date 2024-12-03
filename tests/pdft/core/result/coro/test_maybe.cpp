#include "pdft/core/result.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft {

Maybe<int> just(int x) {
    co_return x;
}

Maybe<int> faulty() {
    return std::nullopt;
}

TEST_CASE("testMaybeMonadOk") {
    auto x = [] -> Maybe<int> {
        int x = co_await just(10);
        int y = co_await just(20);
        co_return x + y;
    }();

    CHECK(x.has_value());
    CHECK(*x == 30);
}

TEST_CASE("testMaybeMonadFail") {
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

TEST_CASE("testMaybeMonadNoAwaitOk") {
    auto x = [] -> Maybe<int> {
        Maybe<int> x = just(10);
        return x;
    }();

    CHECK(x.has_value());
    CHECK(*x == 10);
}

TEST_CASE("testMaybeMonadNoAwaitFaulty") {
    auto x = [] -> Maybe<int> {
        Maybe<int> x = faulty();
        return x.value_or(20);
    }();

    CHECK(x.has_value());
    CHECK(*x == 20);
}

}  // namespace pdft
