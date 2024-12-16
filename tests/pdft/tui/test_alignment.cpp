#include "pdft/tui/alignment.h"

#include <catch2/catch_all.hpp>

namespace pdft::tui::layout {

using enum Alignment;

TEST_CASE("axisFit/int/fills") {
    CHECK(getOffset(10, 10, GENERATE(Min, Center, Max)) == 0);
}

TEST_CASE("axisFit/float/fills") {
    CHECK(getOffset(10.f, 10.f, GENERATE(Min, Center, Max)) == 0.f);
}

TEST_CASE("axisFit/int/min") {
    CHECK(getOffset(10, 5, Min) == 0);
}

TEST_CASE("axisFit/float/min") {
    CHECK(getOffset(10.f, 5.f, Min) == 0.f);
}

TEST_CASE("axisFit/int/center") {
    CHECK(getOffset(10, 5, Center) == 2);
}

TEST_CASE("axisFit/float/center") {
    CHECK(getOffset(10.f, 5.f, Center) == 2.5f);
}

TEST_CASE("axisFit/int/max") {
    CHECK(getOffset(10, 5, Max) == 5);
}

TEST_CASE("axisFit/float/max") {
    CHECK(getOffset(10.f, 5.f, Max) == 5.f);
}

}  // namespace pdft::tui::layout
