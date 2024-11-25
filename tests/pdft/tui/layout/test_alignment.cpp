#include "pdft/tui/layout/alignment.h"

#include <catch2/catch_all.hpp>

namespace pdft::tui::layout {

using enum Alignment;

TEST_CASE("testAxisFitFillsInt", "") {
    CHECK(getOffset(10, 10, GENERATE(Min, Center, Max)) == 0);
}

TEST_CASE("testAxisFitFillsFloat", "") {
    CHECK(getOffset(10.f, 10.f, GENERATE(Min, Center, Max)) == 0.f);
}

TEST_CASE("testAxisFitMinInt", "") {
    CHECK(getOffset(10, 5, Min) == 0);
}

TEST_CASE("testAxisFitMinFloat", "") {
    CHECK(getOffset(10.f, 5.f, Min) == 0.f);
}

TEST_CASE("testAxisFitCenterInt", "") {
    CHECK(getOffset(10, 5, Center) == 2);
}

TEST_CASE("testAxisFitCenterFloat", "") {
    CHECK(getOffset(10.f, 5.f, Center) == 2.5f);
}

TEST_CASE("testAxisFitMaxInt", "") {
    CHECK(getOffset(10, 5, Max) == 5);
}

TEST_CASE("testAxisFitMaxFloat", "") {
    CHECK(getOffset(10.f, 5.f, Max) == 5.f);
}

}  // namespace pdft::tui::layout
