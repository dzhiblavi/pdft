#include "pdft/view/layout.h"

#include "pdft/utils/visit.h"

#include <tuple>

namespace pdft::view {

namespace {

IRect transpose(const IRect& r) {
    return IRect{
        .x = r.y,
        .y = r.x,
        .width = r.height,
        .height = r.width,
    };
}

auto getFillCoeffSumAndAvailableSpace(
    const std::vector<Constraint>& constraints, uint32_t available_space) {
    uint32_t coeffs_sum = 0;
    uint32_t available_fill_space = available_space;

    for (auto& constraint : constraints) {
        utils::visit(
            constraint, [&](constraints::Fill fill) { coeffs_sum += fill.coeff; },
            [&](constraints::Fraction fraction) {
                available_fill_space -= uint32_t(available_space * fraction.fraction);
            },
            [&](constraints::Length length) { available_fill_space -= length.length; });
    }

    return std::make_tuple(coeffs_sum, available_fill_space);
}

auto splitAssumeVertical(const std::vector<Constraint>& constraints, IRect rectangle) {
    std::vector<IRect> rects;
    rects.reserve(constraints.size());

    auto [fill_coeffs_sum, available_fill_space] =
        getFillCoeffSumAndAvailableSpace(constraints, rectangle.height);

    for (auto& constraint : constraints) {
        auto height = utils::visit(
            constraint,
            [&](constraints::Fill fill) {
                auto height = uint32_t(available_fill_space * float(fill.coeff) / fill_coeffs_sum);
                available_fill_space -= height;
                fill_coeffs_sum -= fill.coeff;
                return height;
            },
            [&](constraints::Fraction fraction) {
                return uint32_t(rectangle.height * fraction.fraction);
            },
            [](constraints::Length length) { return length.length; });

        rects.push_back({
            .x = rectangle.x,
            .y = rectangle.y,
            .width = rectangle.width,
            .height = height,
        });

        rectangle.y += height;
    }

    return rects;
}

}  // namespace

std::vector<IRect> Layout::split(IRect rectangle) {
    if (orientation_ != Orientation::Vertical) {
        rectangle = transpose(rectangle);
    }

    auto rects = splitAssumeVertical(constraints_, rectangle);

    if (orientation_ != Orientation::Vertical) {
        for (auto& rect : rects) {
            rect = transpose(rect);
        }
    }

    return rects;
}

}  // namespace pdft::view
