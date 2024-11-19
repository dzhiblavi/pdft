#include "pdft/display/fit.h"

#include "pdft/core/assert.h"

namespace pdft::display {

uint32_t getOffset(uint32_t outer, uint32_t inner, view::AxisFit strategy) {
    PDFT_VERIFY(outer >= inner);

    switch (strategy) {
        using enum view::AxisFit;

        case Min:
            return 0;
        case Max:
            return outer - inner;
        case Center:
            return (outer - inner) / 2;

        default:
            core::fail("getOffset unexpected FitStrategy");
    }
}

struct Fitter {
    float width;
    float height;
    float rect_ratio;
    view::Position position;
    Position pos;
    bool image_ratio_is_greater;

    Fitter(float width, float height, view::Position position)
        : width(width)
        , height(height)
        , position(position) {
        rect_ratio = float(position.rect.width) / position.rect.height;
        image_ratio_is_greater = width * position.rect.height > height * position.rect.width;
    }

    uint32_t ceil(uint64_t a, uint32_t b) {
        return (a + b - 1) / b;
    }

    uint32_t adjustedHeight() {
        return ceil(uint64_t(position.rect.width) * height, width);
    }

    uint32_t adjustedWidth() {
        return ceil(uint64_t(position.rect.height) * width, height);
    }

    void fitInside() {
        uint32_t in_img_cols;
        uint32_t in_img_rows;

        if (image_ratio_is_greater) {
            in_img_rows = adjustedHeight();
            in_img_cols = position.rect.width;

            pos.left = position.rect.x;
            pos.top = position.rect.y + getOffset(position.rect.height, in_img_rows, position.vert);
        } else {
            in_img_rows = position.rect.height;
            in_img_cols = adjustedWidth();

            pos.left = position.rect.x + getOffset(position.rect.width, in_img_cols, position.hor);
            pos.top = position.rect.y;
        }

        pos.columns = in_img_cols;
        pos.rows = in_img_rows;
    }

    Position fit() {
        fitInside();
        return pos;
    }
};

Position fitPosition(float width, float height, view::Position position) {
    return Fitter(width, height, position).fit();
}

}  // namespace pdft::display
