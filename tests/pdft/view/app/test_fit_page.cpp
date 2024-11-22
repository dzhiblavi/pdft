#include "pdft/view/app/fit_page.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <iostream>

namespace pdft::view::app {

using enum PagePlacement::FitMode;
using enum view::AxisFit;

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

TEST_CASE("testFitPageExactlyMatchesProportions", "") {
    auto pos = PagePlacement{
        .hor = GENERATE(Min, Center, Max),        // indifferent
        .vert = GENERATE(Min, Center, Max),       // indifferent
        .fit = GENERATE(FitInside, CropAndFill),  // indifferent
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 40,
    };

    auto [rect, offset] = fitPage(3.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitPageInsideNarrowMin", "") {
    auto pos = PagePlacement{
        .hor = Min,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitPage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitPageInsideNarrowCenter", "") {
    auto pos = PagePlacement{
        .hor = Center,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = FitInside,
    };

    auto expected = IRect{
        .x = 15,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitPage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitPageInsideNarrowMax", "") {
    auto pos = PagePlacement{
        .hor = Max,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = FitInside,
    };

    auto expected = IRect{
        .x = 20,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitPage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitPageInsideWideMin", "") {
    auto pos = PagePlacement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Min,
        .fit = FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 20,
    };

    auto [rect, offset] = fitPage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitPageInsideWideCenter", "") {
    auto pos = PagePlacement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Center,
        .fit = FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 30,
        .width = 30,
        .height = 20,
    };

    auto [rect, offset] = fitPage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitPageInsideWideMax", "") {
    auto pos = PagePlacement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Max,
        .fit = FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 40,
        .width = 30,
        .height = 20,
    };

    auto [rect, offset] = fitPage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitPageCropNarrowMin", "") {
    auto pos = PagePlacement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Min,
        .fit = CropAndFill,
    };

    auto expected_image = FRect{
        .x = 0.f,
        .y = 0.f,
        .width = 1.f,
        .height = 2.f / 3.f,
    };

    auto [rect, offset] = fitPage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPageCropNarrowCenter", "") {
    auto pos = PagePlacement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Center,
        .fit = CropAndFill,
    };

    auto expected_image = FRect{
        .x = 0.f,
        .y = 1.f / 6.f,
        .width = 1.f,
        .height = 2.f / 3.f,
    };

    auto [rect, offset] = fitPage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPageCropNarrowMax", "") {
    auto pos = PagePlacement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Max,
        .fit = CropAndFill,
    };

    auto expected_image = FRect{
        .x = 0.f,
        .y = 1.f / 3.f,
        .width = 1.f,
        .height = 2.f / 3.f,
    };

    auto [rect, offset] = fitPage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPageCropWideMin", "") {
    auto pos = PagePlacement{
        .hor = Min,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = CropAndFill,
    };

    auto expected_image = FRect{
        .x = 0.f,
        .y = 0.f,
        .width = 1.f / 2.f,
        .height = 1.f,
    };

    auto [rect, offset] = fitPage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPageCropWideCenter", "") {
    auto pos = PagePlacement{
        .hor = Center,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = CropAndFill,
    };

    auto expected_image = FRect{
        .x = 1.f / 4.f,
        .y = 0.f,
        .width = 1.f / 2.f,
        .height = 1.f,
    };

    auto [rect, offset] = fitPage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPageCropWideMax", "") {
    auto pos = PagePlacement{
        .hor = Max,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = CropAndFill,
    };

    auto expected_image = FRect{
        .x = 1.f / 2.f,
        .y = 0.f,
        .width = 1.f / 2.f,
        .height = 1.f,
    };

    auto [rect, offset] = fitPage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

}  // namespace pdft::view::app
