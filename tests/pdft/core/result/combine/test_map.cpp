#include "pdft/core/result/combine/map.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep
#include "pdft/core/result/type/maybe.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::result {

TEST_CASE("mapNone") {
    auto r = Maybe<int>();
    auto u = r | map([](int val) { return val * 2; });
    static_assert(std::is_same_v<decltype(u), Maybe<int>>);
    CHECK(!u);
}

TEST_CASE("mapSome") {
    auto r = Maybe<int>(2);
    auto u = r | map([](int val) { return val * 2; });
    static_assert(std::is_same_v<decltype(u), Maybe<int>>);
    CHECK(u == 4);
}

TEST_CASE("mapOk") {
    auto r = Result<int, int>(2);
    auto u = r | map([](int val) { return std::to_string(val); });
    static_assert(std::is_same_v<decltype(u), Result<std::string, int>>);
    CHECK(u == "2");
}

TEST_CASE("mapErr") {
    auto r = Result<int, int>(std::unexpected(2));
    auto u = r | map([](int val) { return std::to_string(val); });
    static_assert(std::is_same_v<decltype(u), Result<std::string, int>>);
    CHECK(!u);
    CHECK(u.error() == 2);
}

TEST_CASE("mapOkUnit") {
    auto r = Result<Unit, int>(unit);
    auto u = r | map([] { return 2; });
    static_assert(std::is_same_v<decltype(u), Result<int, int>>);
    CHECK(u == 2);
}

TEST_CASE("mapErrUnit") {
    auto r = Result<Unit, int>(std::unexpected(2));
    auto u = r | map([] { return 4; });
    static_assert(std::is_same_v<decltype(u), Result<int, int>>);
    CHECK(!u);
    CHECK(u.error() == 2);
}

}  // namespace pdft::result
