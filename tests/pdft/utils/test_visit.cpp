#include "pdft/utils/visit.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::utils {

TEST_CASE("testVisitUnpack", "") {
    std::variant<int, std::tuple<int, int>> v = std::make_tuple(10, 20);

    bool good = false;
    visitUnpack(v, [&](int) { good = false; }, [&](int, int) { good = true; });
    CHECK(good);
}

}  // namespace pdft::utils
