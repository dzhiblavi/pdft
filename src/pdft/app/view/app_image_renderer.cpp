#include "pdft/app/view/impl/app_image_renderer.h"

#include "pdft/image/image.h"
#include "pdft/image/operations.h"

#include "platform/core/result.h"
#include "platform/log.h"
#include "platform/module/register.h"

#include <asio/any_io_executor.hpp>
#include <asio/post.hpp>
#include <asio/thread_pool.hpp>

namespace pdft::app {

using namespace platform;

AppImageRenderer::AppImageRenderer(platform::Arc<const PaletteStorage> palettes)
    : palettes_(std::move(palettes)) {}

void AppImageRenderer::render(
    Context& ctx, const DisplayProperties& display, image::Image& image) const {
    if (display.invert_colors) {
        auto _ = ctx.push("invert_colors");
        image::invertColors(image.mutableSamples());
    }

    if (display.make_bolder) {
        auto _ = ctx.push("make_bolder");
        image::makeBolder(image, display.invert_colors);
    }

    if (display.palette.has_value()) {
        auto _ = ctx.push("apply_palette");

        palettes_->get(*display.palette)
            .and_then([&](image::PaletteRef palette) -> platform::Maybe<platform::Unit> {
                image::applyPalette(image.mutableSamples(), palette);
                return result::unit;
            })
            .or_else([&]() -> platform::Maybe<platform::Unit> {
                log::warn("failed to find a palette with name {}", *display.palette);
                return none;
            });
    }
}

}  // namespace pdft::app

PLATFORM_REGISTER_MODULE(pdft::app::AppImageRenderer);
