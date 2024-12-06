#pragma once

#include "pdft/app/state/palette_storage.h"
#include "pdft/app/view/display_properties.h"

#include "pdft/image/operations.h"
#include "pdft/media/page.h"

#include "pdft/core/log.h"
#include "pdft/core/result/combine/and_then.h"
#include "pdft/core/result/combine/or_else.h"
#include "pdft/core/result/syntax/pipe.h"

#include <asio/any_io_executor.hpp>
#include <asio/post.hpp>
#include <asio/thread_pool.hpp>

namespace pdft::app {

class PageLoader {
 public:
    explicit PageLoader(Arc<const PaletteStorage> palettes) : palettes_(std::move(palettes)) {}

    Arc<image::Image> renderPage(
        Context& ctx, const DisplayProperties& display, const media::Page& page) const {
        auto image = ctx.push("get_image") | page.getImage(display.scaling_coeff);

        if (display.invert_colors) {
            auto _ = ctx.push("invert_colors");
            image::invertColors(image->mutableSamples());
        }

        if (display.make_bolder) {
            auto _ = ctx.push("make_bolder");
            image::makeBolder(*image);
        }

        if (display.palette.has_value()) {
            auto _ = ctx.push("apply_palette");

            palettes_->get(*display.palette)  //
                | result::andThen([&](image::PaletteRef palette) -> Maybe<Unit> {
                      image::applyPalette(image->mutableSamples(), palette);
                      return result::some();
                  })  //
                | result::orElse([&]() -> Maybe<Unit> {
                      log::warn("failed to find a palette with name {}", *display.palette);
                      return result::none;
                  });
        }

        return image;
    }

 private:
    Arc<const PaletteStorage> palettes_;
};

}  // namespace pdft::app
