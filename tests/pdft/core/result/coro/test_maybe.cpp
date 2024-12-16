#include "pdft/core/result.h"
#include "pdft/core/result/coro/maybe.h"  // IWYU pragma: keep

#include <catch2/catch_test_macros.hpp>

namespace pdft {

using namespace result;

Maybe<int> just(int x) {
    co_return x;
}

Maybe<int> faulty() {
    return std::nullopt;
}

TEST_CASE("coro/maybe/some") {
    auto x = [] -> Maybe<int> {
        int x = co_await just(1);
        int y = co_await just(2);
        co_return x + y;
    }();

    CHECK(x.has_value());
    CHECK(*x == 3);
}

TEST_CASE("coro/maybe/none") {
    bool reached = false;

    auto x = [&reached] -> Maybe<int> {
        int x = co_await just(1);
        int y = co_await just(2);
        reached = true;
        co_return x + y + co_await faulty();
    }();

    CHECK(!x.has_value());
    CHECK(reached);
}

}  // namespace pdft
