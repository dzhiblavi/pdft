#include "pdft/view/app/fit_page.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <iostream>

namespace pdft::view::app {

inline const IRect Rect = {
    .x = 10,
    .y = 20,
    .width = 30,
    .height = 40,
};

inline const IRect ZeroOffset = {
    .x = 0,
    .y = 0,
    .width = 30,
    .height = 40,
};

void dump(const IRect& rect) {
    auto [x, y, w, h] = rect;
    std::cout << std::format("IRect[x={},y={},w={},h={}]", x, y, w, h) << std::endl;
}

TEST_CASE("testFitPositionExactlyMatchesProportions", "") {
    auto pos = PagePlacement{
        .hor = view::AxisFit::Center,
        .vert = view::AxisFit::Max,
        .fit = PagePlacement::FitMode::FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 40,
    };

    auto expected_image = IRect{
        .x = 0,
        .y = 0,
        .width = 30,
        .height = 40,
    };

    auto [rect, offset] = fitPage(3.0f, 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPositionCenterVertically", "") {
    auto vert = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = PagePlacement{
        .hor = view::AxisFit::Center,
        .vert = vert,
        .fit = PagePlacement::FitMode::FitInside,
    };

    auto expected = IRect{
        .x = 15,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto expected_image = IRect{
        .x = 0,
        .y = 0,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitPage(2.0f, 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPositionMinVertically", "") {
    auto vert = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = PagePlacement{
        .hor = view::AxisFit::Min,
        .vert = vert,
        .fit = PagePlacement::FitMode::FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto expected_image = IRect{
        .x = 0,
        .y = 0,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitPage(2.0f, 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPositionMaxVertically", "") {
    auto vert = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = PagePlacement{
        .hor = view::AxisFit::Max,
        .vert = vert,
        .fit = PagePlacement::FitMode::FitInside,
    };

    auto expected = IRect{
        .x = 20,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto expected_image = IRect{
        .x = 0,
        .y = 0,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitPage(2.0f, 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPositionCenterHorizontally", "") {
    auto hor = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = PagePlacement{
        .hor = hor,
        .vert = view::AxisFit::Center,
        .fit = PagePlacement::FitMode::FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 28,
        .width = 30,
        .height = 23,
    };

    auto expected_image = IRect{
        .x = 0,
        .y = 0,
        .width = 30,
        .height = 23,
    };

    auto [rect, offset] = fitPage(4.0f, 3.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPositionMinHorizontally", "") {
    auto hor = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = PagePlacement{
        .hor = hor,
        .vert = view::AxisFit::Min,
        .fit = PagePlacement::FitMode::FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 23,
    };

    auto expected_image = IRect{
        .x = 0,
        .y = 0,
        .width = 30,
        .height = 23,
    };

    auto [rect, offset] = fitPage(4.0f, 3.0f, Rect, pos);

    dump(rect);
    dump(expected);

    dump(offset);
    dump(expected_image);

    CHECK(rect == expected);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitPositionMaxHorizontally", "") {
    auto hor = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = PagePlacement{
        .hor = hor,
        .vert = view::AxisFit::Max,
        .fit = PagePlacement::FitMode::FitInside,
    };

    auto expected = IRect{
        .x = 10,
        .y = 37,
        .width = 30,
        .height = 23,
    };

    auto expected_image = IRect{
        .x = 0,
        .y = 0,
        .width = 30,
        .height = 23,
    };

    auto [rect, offset] = fitPage(4.0f, 3.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == expected_image);
}

}  // namespace pdft::view::app
