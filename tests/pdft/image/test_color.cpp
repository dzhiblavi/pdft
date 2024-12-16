#include "pdft/image/color.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::image {

TEST_CASE("rgbFromHex/#FFFFF") {
    auto hex = "#ffffff";
    auto rgb = rgbFromHex(hex);

    CHECK(rgb.r == 255);
    CHECK(rgb.g == 255);
    CHECK(rgb.b == 255);
}

TEST_CASE("rgbFromHex/#000000") {
    auto hex = "#000000";
    auto rgb = rgbFromHex(hex);

    CHECK(rgb.r == 0);
    CHECK(rgb.g == 0);
    CHECK(rgb.b == 0);
}

TEST_CASE("rgbFromHex/#aabb12") {
    auto hex = "#aabb12";
    auto rgb = rgbFromHex(hex);

    CHECK(int(rgb.r) == 170);
    CHECK(int(rgb.g) == 187);
    CHECK(int(rgb.b) == 18);
}

}  // namespace pdft::image
