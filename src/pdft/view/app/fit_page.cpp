#include "pdft/view/app/fit_page.h"

namespace pdft::view::app {

struct Fitter {
    float width;
    float height;
    float rect_ratio;
    const IRect& rect;
    const PagePlacement& place;
    bool image_ratio_is_greater;

    // The result
    IRect pos;

    Fitter(float width, float height, const IRect& rect, const PagePlacement& place)
        : width(width)
        , height(height)
        , rect(rect)
        , place(place) {
        rect_ratio = float(rect.width) / rect.height;
        image_ratio_is_greater = width * rect.height > height * rect.width;
    }

    uint32_t ceil(uint64_t a, uint32_t b) {
        return (a + b - 1) / b;
    }

    uint32_t adjustedHeight() {
        return ceil(uint64_t(rect.width) * height, width);
    }

    uint32_t adjustedWidth() {
        return ceil(uint64_t(rect.height) * width, height);
    }

    void fitInside() {
        uint32_t in_img_cols;
        uint32_t in_img_rows;

        if (image_ratio_is_greater) {
            in_img_rows = adjustedHeight();
            in_img_cols = rect.width;

            pos.x = rect.x;
            pos.y = rect.y + getOffset(rect.height, in_img_rows, place.vert);
        } else {
            in_img_rows = rect.height;
            in_img_cols = adjustedWidth();

            pos.x = rect.x + getOffset(rect.width, in_img_cols, place.hor);
            pos.y = rect.y;
        }

        pos.width = in_img_cols;
        pos.height = in_img_rows;
    }

    IRect fit() {
        fitInside();
        return pos;
    }
};

IRect fitPage(float width, float height, const IRect& rect, const PagePlacement& place) {
    return Fitter(width, height, rect, place).fit();
}

}  // namespace pdft::view::app
