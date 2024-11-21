#include "pdft/view/app/fit_text.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <format>
#include <iostream>

namespace pdft::view::app {

using enum view::AxisFit;
using enum TextPlacement::OverflowMode;

namespace {

IRect makeRect(uint32_t num_lines) {
    return {
        .x = 1,
        .y = 2,
        .width = 3,
        .height = num_lines,
    };
}

void dump(const Line& line) {
    auto [x, y, text] = line;
    std::cout << std::format("Line[x={},y={},text={}]", x, y, text) << std::endl;
}

void check(
    const IRect& rect,
    const TextPlacement& placement,
    std::string_view text,
    const std::vector<Line>& expected_lines) {
    size_t index = 0;

    for (auto line : fitText(rect, placement, text)) {
        dump(line);
        CHECK(index < expected_lines.size());
        dump(expected_lines[index]);
        CHECK(expected_lines[index] == line);
        ++index;
    }
}

struct TestCase {
    AxisFit hor;
    AxisFit vert;
    TextPlacement::OverflowMode overflow;
    uint32_t num_lines = 4;
    std::string_view text = "1234567890";
};

void run(const TestCase& test_case, const std::vector<Line>& expected_lines) {
    auto rect = makeRect(test_case.num_lines);

    auto place = TextPlacement{
        .hor = test_case.hor,
        .vert = test_case.vert,
        .overflow = test_case.overflow,
    };

    check(rect, place, test_case.text, expected_lines);
}

}  // namespace

TEST_CASE("testFitTextShortLineHorMin", "") {
    run(
        {
            .hor = Min,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Truncate,
            .num_lines = 1,
            .text = "1",
        },
        {
            {.x = 1, .y = 2, .text = "1"},
        });
}

TEST_CASE("testFitTextShortLineHorCenter", "") {
    run(
        {
            .hor = Center,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Truncate,
            .num_lines = 1,
            .text = "1",
        },
        {
            {.x = 2, .y = 2, .text = "1"},
        });
}

TEST_CASE("testFitTextShortLineHorMax", "") {
    run(
        {
            .hor = Max,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Truncate,
            .num_lines = 1,
            .text = "1",
        },
        {
            {.x = 3, .y = 2, .text = "1"},
        });
}

TEST_CASE("testFitTextRespectsEmptyLines", "") {
    run(
        {
            .hor = Min,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Truncate,
            .num_lines = 3,
            .text = "\n\n\n",
        },
        {
            {.x = 1, .y = 2, .text = ""},
            {.x = 1, .y = 3, .text = ""},
            {.x = 1, .y = 4, .text = ""},
        });
}

TEST_CASE("testFitTextTruncateLongLineVertMin", "") {
    run(
        {
            .hor = GENERATE(Min, Center, Max),
            .vert = Min,
            .overflow = Truncate,
        },
        {
            {.x = 1, .y = 2, .text = "123"},
        });
}

TEST_CASE("testFitTextTruncateLongLineVertCenter", "") {
    run(
        {
            .hor = GENERATE(Min, Center, Max),
            .vert = Center,
            .overflow = Truncate,
        },
        {
            {.x = 1, .y = 3, .text = "123"},
        });
}

TEST_CASE("testFitTextTruncateLongLineVertMax", "") {
    run(
        {
            .hor = GENERATE(Min, Center, Max),
            .vert = Max,
            .overflow = Truncate,
        },
        {
            {.x = 1, .y = 5, .text = "123"},
        });
}

TEST_CASE("testFitTextWrapLongLineLeft", "") {
    run(
        {
            .hor = Min,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Wrap,
        },
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "456"},
            {.x = 1, .y = 4, .text = "789"},
            {.x = 1, .y = 5, .text = "0"},
        });
}

TEST_CASE("testFitTextWrapLongLineRight", "") {
    run(
        {
            .hor = Max,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Wrap,
        },
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "456"},
            {.x = 1, .y = 4, .text = "789"},
            {.x = 3, .y = 5, .text = "0"},
        });
}

TEST_CASE("testFitTextMultilineHorMin", "") {
    run(
        {
            .hor = Min,
            .vert = GENERATE(Min, Center, Max),
            .overflow = GENERATE(Wrap, Truncate),
            .text = "1\n23\n456\n7890",
        },
        {
            {.x = 1, .y = 2, .text = "1"},
            {.x = 1, .y = 3, .text = "23"},
            {.x = 1, .y = 4, .text = "456"},
            {.x = 1, .y = 5, .text = "789"},
        });
}

TEST_CASE("testFitTextMultilineHorCenter", "") {
    run(
        {
            .hor = Center,
            .vert = GENERATE(Min, Center, Max),
            .overflow = GENERATE(Wrap, Truncate),
            .text = "1\n23\n456\n7890",
        },
        {
            {.x = 2, .y = 2, .text = "1"},
            {.x = 1, .y = 3, .text = "23"},
            {.x = 1, .y = 4, .text = "456"},
            {.x = 1, .y = 5, .text = "789"},
        });
}

TEST_CASE("testFitTextMultilineHorMax", "") {
    run(
        {
            .hor = Max,
            .vert = GENERATE(Min, Center, Max),
            .overflow = GENERATE(Wrap, Truncate),
            .text = "1\n23\n456\n7890",
        },
        {
            {.x = 3, .y = 2, .text = "1"},
            {.x = 2, .y = 3, .text = "23"},
            {.x = 1, .y = 4, .text = "456"},
            {.x = 1, .y = 5, .text = "789"},
        });
}

TEST_CASE("testFitTextTruncateMultiline", "") {
    run(
        {
            .hor = GENERATE(Min, Center, Max),
            .vert = GENERATE(Min, Center, Max),
            .overflow = Truncate,
            .num_lines = 2,
            .text = "1234\n5678",
        },
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "567"},
        });
}

TEST_CASE("testFitTextWrapMultilineHorMin", "") {
    run(
        {
            .hor = Min,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Wrap,
            .text = "1234\n5678",
        },
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 1, .y = 3, .text = "4"},
            {.x = 1, .y = 4, .text = "567"},
            {.x = 1, .y = 5, .text = "8"},
        });
}

TEST_CASE("testFitTextWrapMultilineHorCenter", "") {
    run(
        {
            .hor = Center,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Wrap,
            .text = "1234\n5678",
        },
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 2, .y = 3, .text = "4"},
            {.x = 1, .y = 4, .text = "567"},
            {.x = 2, .y = 5, .text = "8"},
        });
}

TEST_CASE("testFitTextWrapMultilineHorMax", "") {
    run(
        {
            .hor = Max,
            .vert = GENERATE(Min, Center, Max),
            .overflow = Wrap,
            .text = "1234\n5678",
        },
        {
            {.x = 1, .y = 2, .text = "123"},
            {.x = 3, .y = 3, .text = "4"},
            {.x = 1, .y = 4, .text = "567"},
            {.x = 3, .y = 5, .text = "8"},
        });
}

TEST_CASE("testFitTextComplex1", "") {
    run(
        {
            .hor = Max,
            .vert = Center,
            .overflow = Wrap,
            .num_lines = 20,
            .text = "abacaba\ndabacaba\nk\n\naa",
        },
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
