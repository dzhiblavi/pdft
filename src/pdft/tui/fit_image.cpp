#include "pdft/tui/fit_image.h"

namespace pdft::tui {

namespace {

struct Fitter {
    float image_wh_ratio;
    const IRect& rect;
    const Placement& place;
    bool image_ratio_is_greater;

    // The result
    IRect pos;

    Fitter(float image_wh_ratio, const IRect& rect, const Placement& place)
        : image_wh_ratio(image_wh_ratio)
        , rect(rect)
        , place(place) {
        image_ratio_is_greater = image_wh_ratio > float(rect.width) / rect.height;
    }

    float adjustedHeight() {
        return rect.width / image_wh_ratio;
    }

    float adjustedWidth() {
        return rect.height * image_wh_ratio;
    }

    uint32_t adjustedHeightRounded() {
        return std::roundf(adjustedHeight());
    }

    uint32_t adjustedWidthRounded() {
        return std::roundf(adjustedWidth());
    }

    void fitInside() {
        uint32_t in_img_cols;
        uint32_t in_img_rows;

        if (image_ratio_is_greater) {
            in_img_rows = adjustedHeightRounded();
            in_img_cols = rect.width;

            pos.x = rect.x;
            pos.y = rect.y + getOffset(rect.height, in_img_rows, place.vert);
        } else {
            in_img_rows = rect.height;
            in_img_cols = adjustedWidthRounded();

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

}  // namespace

IRect fitImage(float image_wh_cell_ratio, const IRect& rect, const Placement& place) {
    return Fitter(image_wh_cell_ratio, rect, place).fit();
}

}  // namespace pdft::tui
