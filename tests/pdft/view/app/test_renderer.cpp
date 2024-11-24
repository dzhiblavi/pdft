#include "pdft/view/app/renderer.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::view::app {

namespace {

const core::terminal::Dimensions TermSize = {
    .rows = 40,
    .columns = 200,
    .height_px = 1080,
    .width_px = 1920,
    .cell_height_px = 1080 / 40,
    .cell_width_px = 1920 / 200,
};

}  // namespace

TEST_CASE("testRendererExample", "") {
    Renderer r(TermSize, {}, {}, {});
}

}  // namespace pdft::view::app
