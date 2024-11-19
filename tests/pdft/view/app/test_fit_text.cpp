#include "pdft/view/app/fit_text.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <format>
#include <iostream>

namespace pdft::view::app {

using enum view::AxisFit;
using enum TextPlacement::OverflowMode;

namespace {

void dump(const Line& line) {
    auto [x, y, text] = line;
    std::cout << std::format("Line[x={},y={},text={}]", x, y, text) << std::endl;
}

void check(const TextPlacement& placement, const std::vector<Line>& expected_lines) {
    size_t index = 0;

    for (auto line : fitText(placement)) {
        dump(line);
        CHECK(index < expected_lines.size());
        dump(expected_lines[index]);
        CHECK(expected_lines[index] == line);
        ++index;
    }
}

TextPlacement testCase(
    view::AxisFit hor,
    view::AxisFit vert,
    TextPlacement::OverflowMode mode,
    std::string_view text = "1234567890",
    uint32_t num_lines = 4) {
    return {
        .position =
            {
                .rect =
                    {
                        .x = 1,
                        .y = 2,
                        .width = 3,
                        .height = num_lines,
                    },
                .hor = hor,
                .vert = vert,
            },
        .overflow = mode,
        .text = text,
    };
}

}  // namespace

TEST_CASE("testFitTextShortLineHorMin", "") {
    check(
        testCase(Min, GENERATE(Min, Center, Max), Truncate, "1", 1),
        {
            {.x = 1, .y = 2, .text = "1"},
        });
}

TEST_CASE("testFitTextShortLineHorCenter", "") {
    check(
        testCase(Center, GENERATE(Min, Center, Max), Truncate, "1", 1),
        {
            {.x = 2, .y = 2, .text = "1"},
        });
}

TEST_CASE("testFitTextShortLineHorMax", "") {
    check(
        testCase(Max, GENERATE(Min, Center, Max), Truncate, "1", 1),
        {
            {.x = 3, .y = 2, .text = "1"},
        });
}

TEST_CASE("testFitTextRespectsEmptyLines", "") {
    check(
        testCase(Min, GENERATE(Min, Center, Max), Truncate, "\n\n\n", 3),
        {
            {.x = 1, .y = 2, .text = ""},
            {.x = 1, .y = 3, .text = ""},
            {.x = 1, .y = 4, .text = ""},
        });
}

TEST_CASE("testFitTextTruncateLongLineVertMin", "") {
    check(
        testCase(GENERATE(Min, Center, Max), Min, Truncate),
        {
            {.x = 1, .y = 2, .text = "123"},
        });
}

TEST_CASE("testFitTextTruncateLongLineVertCenter", "") {
    check(
        testCase(GENERATE(Min, Center, Max), Center, Truncate),
        {
            {.x = 1, .y = 3, .text = "123"},
        });
}

TEST_CASE("testFitTextTruncateLongLineVertMax", "") {
    check(
        testCase(GENERATE(Min, Center, Max), Max, Truncate),
        {
            {.x = 1, .y = 5, .text = "123"},
        });
}

TEST_CASE("testFitTextWrapLongLineLeft", "") {
    check(
        testCase(Min, GENERATE(Min, Center, Max), Wrap),
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "456"},
            {.x = 1, .y = 4, .text = "789"},
            {.x = 1, .y = 5, .text = "0"},
        });
}

TEST_CASE("testFitTextWrapLongLineRight", "") {
    check(
        testCase(Max, GENERATE(Min, Center, Max), Wrap),
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "456"},
            {.x = 1, .y = 4, .text = "789"},
            {.x = 3, .y = 5, .text = "0"},
        });
}

TEST_CASE("testFitTextMultilineHorMin", "") {
    check(
        testCase(Min, GENERATE(Min, Center, Max), GENERATE(Wrap, Truncate), "1\n23\n456\n7890"),
        {
            {.x = 1, .y = 2, .text = "1"},
            {.x = 1, .y = 3, .text = "23"},
            {.x = 1, .y = 4, .text = "456"},
            {.x = 1, .y = 5, .text = "789"},
        });
}

TEST_CASE("testFitTextMultilineHorCenter", "") {
    check(
        testCase(Center, GENERATE(Min, Center, Max), GENERATE(Wrap, Truncate), "1\n23\n456\n7890"),
        {
            {.x = 2, .y = 2, .text = "1"},
            {.x = 1, .y = 3, .text = "23"},
            {.x = 1, .y = 4, .text = "456"},
            {.x = 1, .y = 5, .text = "789"},
        });
}

TEST_CASE("testFitTextMultilineHorMax", "") {
    check(
        testCase(Max, GENERATE(Min, Center, Max), GENERATE(Wrap, Truncate), "1\n23\n456\n7890"),
        {
            {.x = 3, .y = 2, .text = "1"},
            {.x = 2, .y = 3, .text = "23"},
            {.x = 1, .y = 4, .text = "456"},
            {.x = 1, .y = 5, .text = "789"},
        });
}

TEST_CASE("testFitTextTruncateMultiline", "") {
    check(
        testCase(GENERATE(Min, Center, Max), GENERATE(Min, Center, Max), Truncate, "1234\n5678", 2),
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "567"},
        });
}

TEST_CASE("testFitTextWrapMultilineHorMin", "") {
    check(
        testCase(Min, GENERATE(Min, Center, Max), Wrap, "1234\n5678"),
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "4"},
            {.x = 1, .y = 4, .text = "567"},
            {.x = 1, .y = 5, .text = "8"},
        });
}

TEST_CASE("testFitTextWrapMultilineHorCenter", "") {
    check(
        testCase(Center, GENERATE(Min, Center, Max), Wrap, "1234\n5678"),
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 2, .y = 3, .text = "4"},
            {.x = 1, .y = 4, .text = "567"},
            {.x = 2, .y = 5, .text = "8"},
        });
}

TEST_CASE("testFitTextWrapMultilineHorMax", "") {
    check(
        testCase(Max, GENERATE(Min, Center, Max), Wrap, "1234\n5678"),
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 3, .y = 3, .text = "4"},
            {.x = 1, .y = 4, .text = "567"},
            {.x = 3, .y = 5, .text = "8"},
        });
}

TEST_CASE("testFitTextComplex1", "") {
    check(
        testCase(Max, Center, Wrap, "abacaba\ndabacaba\nk\n\naa", 20),
        {
            {.x = 1, .y = 7, .text = "aba"},
            {.x = 1, .y = 8, .text = "cab"},
            {.x = 3, .y = 9, .text = "a"},
            {.x = 1, .y = 10, .text = "dab"},
            {.x = 1, .y = 11, .text = "aca"},
            {.x = 2, .y = 12, .text = "ba"},
            {.x = 3, .y = 13, .text = "k"},
            {.x = 4, .y = 14, .text = ""},
            {.x = 2, .y = 15, .text = "aa"},
        });
}

}  // namespace pdft::view::app
