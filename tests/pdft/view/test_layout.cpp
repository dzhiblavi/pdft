#include "pdft/view/layout.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

namespace pdft::view {

inline const IRect TestRect = IRect{
    .x = 10,
    .y = 20,
    .width = 30,
    .height = 40,
};

TEST_CASE("testFillOneHorizontal", "") {
    auto layout = horizontal(constraints::Fill{1});
    CHECK(layout.split(TestRect) == std::vector{TestRect});
}

TEST_CASE("testFillOneVertical", "") {
    auto layout = vertical(constraints::Fill{1});
    CHECK(layout.split(TestRect) == std::vector{TestRect});
}

TEST_CASE("testFillMultipleHorizontal", "") {
    auto layout = horizontal(constraints::Fill{1}, constraints::Fill{2}, constraints::Fill{3});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 5, 40},
            IRect{15, 20, 10, 40},
            IRect{25, 20, 15, 40},
        });
}

TEST_CASE("testFillMultipleVertical", "") {
    auto layout = vertical(constraints::Fill{1}, constraints::Fill{2}, constraints::Fill{5});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 30, 5},
            IRect{10, 25, 30, 10},
            IRect{10, 35, 30, 25},
        });
}

TEST_CASE("testFractionOneHorizontal", "") {
    auto layout = horizontal(constraints::Fraction{0.5f});
    CHECK(layout.split(TestRect) == std::vector{IRect{10, 20, 15, 40}});
}

TEST_CASE("testFractionOneVertical", "") {
    auto layout = vertical(constraints::Fraction{0.5f});
    CHECK(layout.split(TestRect) == std::vector{IRect{10, 20, 30, 20}});
}

TEST_CASE("testFractionMultipleHorizontal", "") {
    using constraints::Fraction;
    auto layout = horizontal(Fraction{0.5f}, Fraction{0.2f}, Fraction{0.2f});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 15, 40},
            IRect{25, 20, 6, 40},
            IRect{31, 20, 6, 40},
        });
}

TEST_CASE("testFractionMultipleVertical", "") {
    using constraints::Fraction;
    auto layout = vertical(Fraction{0.5f}, Fraction{0.2f}, Fraction{0.2f});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 30, 20},
            IRect{10, 40, 30, 8},
            IRect{10, 48, 30, 8},
        });
}

TEST_CASE("testLengthOneHorizontal", "") {
    auto layout = horizontal(constraints::Length{10});
    CHECK(layout.split(TestRect) == std::vector{IRect{10, 20, 10, 40}});
}

TEST_CASE("testLengthOneVertical", "") {
    auto layout = vertical(constraints::Length{20});
    CHECK(layout.split(TestRect) == std::vector{IRect{10, 20, 30, 20}});
}

TEST_CASE("testLengthMultipleHorizontal", "") {
    using constraints::Length;
    auto layout = horizontal(Length{5}, Length{10}, Length{3});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 5, 40},
            IRect{15, 20, 10, 40},
            IRect{25, 20, 3, 40},
        });
}

TEST_CASE("testLengthMultipleVertical", "") {
    using constraints::Length;
    auto layout = vertical(Length{5}, Length{10}, Length{3});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 30, 5},
            IRect{10, 25, 30, 10},
            IRect{10, 35, 30, 3},
        });
}

TEST_CASE("testLengthAndFillVertical", "") {
    using namespace constraints;

    auto layout = vertical(Length{35}, Fill{1});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 30, 35},
            IRect{10, 55, 30, 5},
        });
}

TEST_CASE("testMixedMultipleHorizontal", "") {
    using namespace constraints;
    auto layout = horizontal(Length{2}, Fill{1}, Fraction{0.5f}, Fill{2}, Length{3});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 2, 40},
            IRect{12, 20, 3, 40},
            IRect{15, 20, 15, 40},
            IRect{30, 20, 7, 40},
            IRect{37, 20, 3, 40},
        });
}

TEST_CASE("testMixedMultipleVertical", "") {
    using namespace constraints;
    auto layout = vertical(Length{2}, Fill{1}, Fraction{0.5f}, Fill{2}, Length{3});
    CHECK(
        layout.split(TestRect) ==
        std::vector{
            IRect{10, 20, 30, 2},
            IRect{10, 22, 30, 5},
            IRect{10, 27, 30, 20},
            IRect{10, 47, 30, 10},
            IRect{10, 57, 30, 3},
        });
}

}  // namespace pdft::view
