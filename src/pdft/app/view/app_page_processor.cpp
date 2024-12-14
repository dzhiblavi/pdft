#include "pdft/app/view/impl/app_page_processor.h"

#include "pdft/core/module/register.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/utils/zlib.h"

#include <asio/bind_executor.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::app {

AppPageProcessor::AppPageProcessor(
    Config config, inject<const ImageRenderer, "img_renderer"> renderer)
    : renderer_(std::move(renderer))
    , pool_(config.render_pool_size)
    , media_strand_(pool_.get_executor()) {}

AppPageProcessor::~AppPageProcessor() noexcept {
    pool_.stop();
    pool_.join();
}

void AppPageProcessor::process(
    Arc<media::Page> page, const DisplayProperties& display, Promise<ProcessedImage> promise) {
    asio::co_spawn(
        media_strand_,
        // NOLINTNEXTLINE
        [this, page, display, p = std::move(promise)]() mutable -> asio::awaitable<void> {
            auto tracer = trace::makeTracer("bg_task");
            Context ctx(tracer);

            auto image = ctx.push("get_image") | page->getImage(display.scaling_coeff);

            {
                auto _ = ctx.push("jump_to_pool");
                co_await asio::post(asio::bind_executor(pool_, asio::use_awaitable));
            }

            {
                auto _ = ctx.push("render_image");
                renderer_->render(ctx, display, *image);
            }

            auto compressed = ctx.push("compress") |
                              utils::zlibCompress(image::pixelsToSamples(image->samples()));

            trace::logStackTrace(std::move(tracer), log::loggerOrGlobal("bg-trace"));

            std::move(p).set_value(ProcessedImage{
                .size = image->size(),
                .compressed_samples = std::move(compressed),
            });
        },
        asio::detached);
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppPageProcessor);
