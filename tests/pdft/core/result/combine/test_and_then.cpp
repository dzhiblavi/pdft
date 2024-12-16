#include "pdft/core/result/combine/and_then.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep

#include <catch2/catch_test_macros.hpp>

namespace pdft::result {

template <typename T>
using Res = Result<T, int>;

template <typename T>
Res<T> makeOk(T val) {
    return val;
}

template <typename T>
Res<T> makeErr(T val) {
    return err(val);
}

TEST_CASE("andThen/value/ok") {
    auto r = makeOk(1);
    auto u = r | andThen([](int val) -> Res<std::string> { return std::to_string(val); });
    CHECK(u);
    CHECK(*u == "1");
}

TEST_CASE("andThen/value/err") {
    auto r = makeErr(1);
    auto u = r | andThen([](int val) -> Res<std::string> { return std::to_string(val); });
    CHECK(!u);
    CHECK(u.error() == 1);
}

TEST_CASE("andThen/unit/ok") {
    auto r = Res<Unit>(unit);
    auto u = r | andThen([]() -> Res<int> { return 2; });
    CHECK(u);
    CHECK(*u == 2);
}

TEST_CASE("andThen/unit/err") {
    auto r = Res<Unit>(std::unexpected(1));
    auto u = r | andThen([]() -> Res<int> { return 2; });
    CHECK(!u);
    CHECK(u.error() == 1);
}

TEST_CASE("andThen/some") {
    auto r = Maybe<int>(1);
    auto u = r | andThen([](int val) -> Maybe<int> { return 2 * val; });
    CHECK(u == 2);
}

TEST_CASE("andThen/none") {
    auto r = Maybe<int>();
    auto u = r | andThen([](int val) -> Maybe<int> { return 2 * val; });
    CHECK(!u);
}

}  // namespace pdft::result
