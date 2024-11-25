#include "pdft/tui/image/fit_image.h"

namespace pdft::tui::image {

struct Fitter {
    float wh_ratio;
    const layout::IRect& rect;
    const Placement& place;
    bool image_ratio_is_greater;

    // The result
    layout::IRect pos;
    layout::FRect image;

    Fitter(float wh_ratio, const layout::IRect& rect, const Placement& place)
        : wh_ratio(wh_ratio)
        , rect(rect)
        , place(place) {
        image_ratio_is_greater = wh_ratio * rect.height > rect.width;
    }

    float adjustedHeight() {
        return rect.width / wh_ratio;
    }

    float adjustedWidth() {
        return rect.height * wh_ratio;
    }

    uint32_t adjustedHeightRounded() {
        return std::roundf(adjustedHeight());
    }

    uint32_t adjustedWidthRounded() {
        return std::roundf(adjustedWidth());
    }

    void fitInside() {
        // Display the full image with no offset
        image.x = image.y = 0.f;
        image.width = image.height = 1.f;

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

    void fitCropAndFill() {
        pos = rect;

        if (image_ratio_is_greater) {
            float img_width = adjustedWidth();

            image = {
                .x = getOffset(img_width, float(rect.width), place.hor) / img_width,
                .y = 0.f,  // no vertical offset
                .width = rect.width / img_width,
                .height = 1.f,  // full height
            };
        } else {
            float img_height = adjustedHeight();

            image = {
                .x = 0.f,  // no horizontal offset
                .y = getOffset(img_height, float(rect.height), place.vert) / img_height,
                .width = 1.f,  // full width
                .height = rect.height / img_height,
            };
        }
    }

    std::tuple<layout::IRect, layout::FRect> fit() {
        switch (place.fit) {
            using enum Placement::FitMode;

            case FitInside:
                fitInside();
                break;

            case CropAndFill:
                fitCropAndFill();
                break;
        }

        return std::make_tuple(pos, image);
    }
};

std::tuple<layout::IRect, layout::FRect> fitImage(
    float wh_ratio, const layout::IRect& rect, const Placement& place) {
    return Fitter(wh_ratio, rect, place).fit();
}

}  // namespace pdft::tui::image
