#include "pdft/tui/image/fit_image.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <iostream>

namespace pdft::tui::image {

using enum Alignment;

inline const IRect Rect = {
    .x = 10,
    .y = 20,
    .width = 30,
    .height = 40,
};

inline const FRect FullImage = {
    .x = 0.f,
    .y = 0.f,
    .width = 1.f,
    .height = 1.f,
};

void dump(const IRect& rect) {
    auto [x, y, w, h] = rect;
    std::cout << std::format("IRect[x={},y={},w={},h={}]", x, y, w, h) << std::endl;
}

TEST_CASE("testFitImageExactlyMatchesProportions", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),   // indifferent
        .vert = GENERATE(Min, Center, Max),  // indifferent
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 40,
    };

    auto rect = fitImage(3.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
}

TEST_CASE("testFitImageInsideNarrowMin", "") {
    auto pos = Placement{
        .hor = Min, .vert = GENERATE(Min, Center, Max),  // indifferent
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto rect = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
}

TEST_CASE("testFitImageInsideNarrowCenter", "") {
    auto pos = Placement{
        .hor = Center, .vert = GENERATE(Min, Center, Max),  // indifferent
    };

    auto expected = IRect{
        .x = 15,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto rect = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
}

TEST_CASE("testFitImageInsideNarrowMax", "") {
    auto pos = Placement{
        .hor = Max, .vert = GENERATE(Min, Center, Max),  // indifferent
    };

    auto expected = IRect{
        .x = 20,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto rect = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
}

TEST_CASE("testFitImageInsideWideMin", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Min,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 20,
    };

    auto rect = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
}

TEST_CASE("testFitImageInsideWideCenter", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Center,
    };

    auto expected = IRect{
        .x = 10,
        .y = 30,
        .width = 30,
        .height = 20,
    };

    auto rect = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
}

TEST_CASE("testFitImageInsideWideMax", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Max,
    };

    auto expected = IRect{
        .x = 10,
        .y = 40,
        .width = 30,
        .height = 20,
    };

    auto rect = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
}

}  // namespace pdft::tui::image
