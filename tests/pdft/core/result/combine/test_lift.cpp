#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep
#include "pdft/core/result/type/maybe.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::result {

TEST_CASE("lift/none") {
    auto r = Maybe<int>();
    auto u = r | lift(2);

    static_assert(std::is_same_v<decltype(u), Result<int, int>>);
    CHECK(!u);
    CHECK(u.error() == 2);
}

TEST_CASE("lift/some") {
    auto r = Maybe<int>(1);
    auto u = r | lift(2);

    static_assert(std::is_same_v<decltype(u), Result<int, int>>);
    CHECK(u == 1);
}

}  // namespace pdft::result
