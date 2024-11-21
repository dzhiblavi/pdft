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
    IRect image;

    Fitter(float width, float height, const IRect& rect, const PagePlacement& place)
        : width(width)
        , height(height)
        , rect(rect)
        , place(place) {
        rect_ratio = float(rect.width) / rect.height;
        image_ratio_is_greater = width * rect.height > height * rect.width;
    }

    uint32_t adjustedHeight() {
        return std::ceil(float(rect.width) * height / width);
    }

    uint32_t adjustedWidth() {
        return std::ceil(float(rect.height) * width / height);
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

        image.width = pos.width = in_img_cols;
        image.height = pos.height = in_img_rows;
        image.x = image.y = 0;
    }

    void fitCropAndFill() {
        pos = rect;

        if (image_ratio_is_greater) {
            image = {
                .x = static_cast<int32_t>(getOffset(adjustedWidth(), rect.width, place.hor)),
                .y = 0,
                .width = adjustedWidth(),
                .height = rect.height,
            };
        } else {
            image = {
                .x = 0,
                .y = static_cast<int32_t>(getOffset(adjustedHeight(), rect.height, place.vert)),
                .width = rect.width,
                .height = adjustedHeight(),
            };
        }
    }

    std::tuple<IRect, IRect> fit() {
        switch (place.fit) {
            using enum PagePlacement::FitMode;

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

std::tuple<IRect, IRect> fitPage(
    float width, float height, const IRect& rect, const PagePlacement& place) {
    return Fitter(width, height, rect, place).fit();
}

}  // namespace pdft::view::app
