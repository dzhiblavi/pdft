#include "pdft/view/app/fit_image.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace pdft::view::app {

TEST_CASE("testFitPositionExactlyMatchesProportions", "") {
    auto pos = view::Position{
        .rect =
            {
                .x = 10,
                .y = 20,
                .width = 30,
                .height = 40,
            },
        .hor = view::AxisFit::Center,
        .vert = view::AxisFit::Max,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 40,
    };

    CHECK(fitImage(3.0f, 4.0f, pos) == expected);
}

TEST_CASE("testFitPositionCenterVertically", "") {
    auto vert = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = view::Position{
        .rect =
            {
                .x = 10,
                .y = 20,
                .width = 30,
                .height = 40,
            },
        .hor = view::AxisFit::Center,
        .vert = vert,
    };

    auto expected = IRect{
        .x = 15,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    CHECK(fitImage(2.0f, 4.0f, pos) == expected);
}

TEST_CASE("testFitPositionMinVertically", "") {
    auto vert = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = view::Position{
        .rect =
            {
                .x = 10,
                .y = 20,
                .width = 30,
                .height = 40,
            },
        .hor = view::AxisFit::Min,
        .vert = vert,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    CHECK(fitImage(2.0f, 4.0f, pos) == expected);
}

TEST_CASE("testFitPositionMaxVertically", "") {
    auto vert = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = view::Position{
        .rect =
            {
                .x = 10,
                .y = 20,
                .width = 30,
                .height = 40,
            },
        .hor = view::AxisFit::Max,
        .vert = vert,
    };

    auto expected = IRect{
        .x = 20,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    CHECK(fitImage(2.0f, 4.0f, pos) == expected);
}

TEST_CASE("testFitPositionCenterHorizontally", "") {
    auto hor = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = view::Position{
        .rect =
            {
                .x = 10,
                .y = 20,
                .width = 30,
                .height = 40,
            },
        .hor = hor,
        .vert = view::AxisFit::Center,
    };

    auto expected = IRect{
        .x = 10,
        .y = 28,
        .width = 30,
        .height = 23,
    };

    CHECK(fitImage(4.0f, 3.0f, pos) == expected);
}

TEST_CASE("testFitPositionMinHorizontally", "") {
    auto hor = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = view::Position{
        .rect =
            {
                .x = 10,
                .y = 20,
                .width = 30,
                .height = 40,
            },
        .hor = hor,
        .vert = view::AxisFit::Min,
    };

    auto expected = IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 23,
    };

    CHECK(fitImage(4.0f, 3.0f, pos) == expected);
}

TEST_CASE("testFitPositionMaxHorizontally", "") {
    auto hor = GENERATE(view::AxisFit::Center, view::AxisFit::Max, view::AxisFit::Min);

    auto pos = view::Position{
        .rect =
            {
                .x = 10,
                .y = 20,
                .width = 30,
                .height = 40,
            },
        .hor = hor,
        .vert = view::AxisFit::Max,
    };

    auto expected = IRect{
        .x = 10,
        .y = 37,
        .width = 30,
        .height = 23,
    };

    CHECK(fitImage(4.0f, 3.0f, pos) == expected);
}

}  // namespace pdft::view::app
