#include "pdft/tui/image/fit_image.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <iostream>

namespace pdft::tui::image {

using enum Placement::FitMode;
using enum layout::Alignment;

inline const layout::IRect Rect = {
    .x = 10,
    .y = 20,
    .width = 30,
    .height = 40,
};

inline const layout::FRect FullImage = {
    .x = 0.f,
    .y = 0.f,
    .width = 1.f,
    .height = 1.f,
};

void dump(const layout::IRect& rect) {
    auto [x, y, w, h] = rect;
    std::cout << std::format("IRect[x={},y={},w={},h={}]", x, y, w, h) << std::endl;
}

TEST_CASE("testFitImageExactlyMatchesProportions", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),        // indifferent
        .vert = GENERATE(Min, Center, Max),       // indifferent
        .fit = GENERATE(FitInside, CropAndFill),  // indifferent
    };

    auto expected = layout::IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 40,
    };

    auto [rect, offset] = fitImage(3.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitImageInsideNarrowMin", "") {
    auto pos = Placement{
        .hor = Min,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = FitInside,
    };

    auto expected = layout::IRect{
        .x = 10,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitImageInsideNarrowCenter", "") {
    auto pos = Placement{
        .hor = Center,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = FitInside,
    };

    auto expected = layout::IRect{
        .x = 15,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitImageInsideNarrowMax", "") {
    auto pos = Placement{
        .hor = Max,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = FitInside,
    };

    auto expected = layout::IRect{
        .x = 20,
        .y = 20,
        .width = 20,
        .height = 40,
    };

    auto [rect, offset] = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitImageInsideWideMin", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Min,
        .fit = FitInside,
    };

    auto expected = layout::IRect{
        .x = 10,
        .y = 20,
        .width = 30,
        .height = 20,
    };

    auto [rect, offset] = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitImageInsideWideCenter", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Center,
        .fit = FitInside,
    };

    auto expected = layout::IRect{
        .x = 10,
        .y = 30,
        .width = 30,
        .height = 20,
    };

    auto [rect, offset] = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitImageInsideWideMax", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Max,
        .fit = FitInside,
    };

    auto expected = layout::IRect{
        .x = 10,
        .y = 40,
        .width = 30,
        .height = 20,
    };

    auto [rect, offset] = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == expected);
    CHECK(offset == FullImage);
}

TEST_CASE("testFitImageCropNarrowMin", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Min,
        .fit = CropAndFill,
    };

    auto expected_image = layout::FRect{
        .x = 0.f,
        .y = 0.f,
        .width = 1.f,
        .height = 2.f / 3.f,
    };

    auto [rect, offset] = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitImageCropNarrowCenter", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Center,
        .fit = CropAndFill,
    };

    auto expected_image = layout::FRect{
        .x = 0.f,
        .y = 1.f / 6.f,
        .width = 1.f,
        .height = 2.f / 3.f,
    };

    auto [rect, offset] = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitImageCropNarrowMax", "") {
    auto pos = Placement{
        .hor = GENERATE(Min, Center, Max),  // indifferent
        .vert = Max,
        .fit = CropAndFill,
    };

    auto expected_image = layout::FRect{
        .x = 0.f,
        .y = 1.f / 3.f,
        .width = 1.f,
        .height = 2.f / 3.f,
    };

    auto [rect, offset] = fitImage(2.0f / 4.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitImageCropWideMin", "") {
    auto pos = Placement{
        .hor = Min,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = CropAndFill,
    };

    auto expected_image = layout::FRect{
        .x = 0.f,
        .y = 0.f,
        .width = 1.f / 2.f,
        .height = 1.f,
    };

    auto [rect, offset] = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitImageCropWideCenter", "") {
    auto pos = Placement{
        .hor = Center,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = CropAndFill,
    };

    auto expected_image = layout::FRect{
        .x = 1.f / 4.f,
        .y = 0.f,
        .width = 1.f / 2.f,
        .height = 1.f,
    };

    auto [rect, offset] = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

TEST_CASE("testFitImageCropWideMax", "") {
    auto pos = Placement{
        .hor = Max,
        .vert = GENERATE(Min, Center, Max),  // indifferent
        .fit = CropAndFill,
    };

    auto expected_image = layout::FRect{
        .x = 1.f / 2.f,
        .y = 0.f,
        .width = 1.f / 2.f,
        .height = 1.f,
    };

    auto [rect, offset] = fitImage(3.0f / 2.0f, Rect, pos);
    CHECK(rect == Rect);
    CHECK(offset == expected_image);
}

}  // namespace pdft::tui::image
