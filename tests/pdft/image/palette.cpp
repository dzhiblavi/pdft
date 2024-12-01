#include "pdft/image/palette.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::image {

TEST_CASE("testPalette") {
    auto palette = getDefaultPalette();
    (void)palette;
}

}  // namespace pdft::image
