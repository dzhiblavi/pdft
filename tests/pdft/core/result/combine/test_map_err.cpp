#include "pdft/core/result/combine/map_err.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep

#include <catch2/catch_test_macros.hpp>

namespace pdft::result {

TEST_CASE("mapErr/value/err") {
    auto r = Result<int, int>(std::unexpected(2));
    auto u = r | mapErr([](int val) { return val * 2L; });
    static_assert(std::is_same_v<decltype(u), Result<int, long>>);
    CHECK(!u);
    CHECK(u.error() == 4U);
}

TEST_CASE("mapErr/value/ok") {
    auto r = Result<int, int>(2);
    auto u = r | mapErr([](int val) { return val * 2L; });
    static_assert(std::is_same_v<decltype(u), Result<int, long>>);
    CHECK(u == 2);
}

TEST_CASE("mapErr/unit/err") {
    auto r = Result<Unit, int>(std::unexpected(2));
    auto u = r | mapErr([](int val) { return val * 2L; });
    static_assert(std::is_same_v<decltype(u), Result<Unit, long>>);
    CHECK(!u);
    CHECK(u.error() == 4UL);
}

TEST_CASE("mapErr/unit/ok") {
    auto r = Result<Unit, int>(unit);
    auto u = r | mapErr([](int val) { return val * 2L; });
    static_assert(std::is_same_v<decltype(u), Result<Unit, long>>);
    CHECK(u);
}

}  // namespace pdft::result
