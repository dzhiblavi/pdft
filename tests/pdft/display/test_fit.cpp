#include "pdft/display/fit.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace pdft::display {

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

    auto expected = Position{
        .rows = 40,
        .columns = 30,
        .top = 20,
        .left = 10,
    };

    CHECK(fitPosition(3.0f, 4.0f, pos) == expected);
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

    auto expected = Position{
        .rows = 40,
        .columns = 20,
        .top = 20,
        .left = 15,
    };

    CHECK(fitPosition(2.0f, 4.0f, pos) == expected);
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

    auto expected = Position{
        .rows = 40,
        .columns = 20,
        .top = 20,
        .left = 10,
    };

    CHECK(fitPosition(2.0f, 4.0f, pos) == expected);
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

    auto expected = Position{
        .rows = 40,
        .columns = 20,
        .top = 20,
        .left = 20,
    };

    CHECK(fitPosition(2.0f, 4.0f, pos) == expected);
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

    auto expected = Position{
        .rows = 23,
        .columns = 30,
        .top = 28,
        .left = 10,
    };

    CHECK(fitPosition(4.0f, 3.0f, pos) == expected);
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

    auto expected = Position{
        .rows = 23,
        .columns = 30,
        .top = 20,
        .left = 10,
    };

    CHECK(fitPosition(4.0f, 3.0f, pos) == expected);
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

    auto expected = Position{
        .rows = 23,
        .columns = 30,
        .top = 37,
        .left = 10,
    };

    CHECK(fitPosition(4.0f, 3.0f, pos) == expected);
}

}  // namespace pdft::display
