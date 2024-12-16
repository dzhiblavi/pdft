#include "pdft/core/result/combine/or_else.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep
#include "pdft/core/result/type/unit.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::result {

TEST_CASE("orElseErr") {
    auto r = Result<int, int>(std::unexpected(2));
    auto u = r | orElse([](int err) -> Result<int, int> { return err * 2; });
    static_assert(std::is_same_v<decltype(u), Result<int, int>>);
    CHECK(u == 4);
}

TEST_CASE("orElseOk") {
    auto r = Result<int, int>(2);
    auto u = r | orElse([](int err) -> Result<int, int> { return err * 2; });
    static_assert(std::is_same_v<decltype(u), Result<int, int>>);
    CHECK(u == 2);
}

TEST_CASE("orElseErrUnit") {
    auto r = Result<int, Unit>(std::unexpected(unit));
    auto u = r | orElse([]() -> Result<int, Unit> { return 1; });
    static_assert(std::is_same_v<decltype(u), Result<int, Unit>>);
    CHECK(u == 1);
}

TEST_CASE("orElseOkUnit") {
    auto r = Result<int, Unit>(1);
    auto u = r | orElse([]() -> Result<int, Unit> { return 2; });
    static_assert(std::is_same_v<decltype(u), Result<int, Unit>>);
    CHECK(u == 1);
}

TEST_CASE("orElseNone") {
    auto r = Maybe<int>();
    auto u = r | orElse([]() -> Maybe<int> { return 1; });
    static_assert(std::is_same_v<decltype(u), Maybe<int>>);
    CHECK(u == 1);
}

TEST_CASE("orElseSome") {
    auto r = Maybe<int>(1);
    auto u = r | orElse([]() -> Maybe<int> { return 2; });
    static_assert(std::is_same_v<decltype(u), Maybe<int>>);
    CHECK(u == 1);
}

}  // namespace pdft::result
