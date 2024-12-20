#include "pdft/app/view/image_renderer.h"
#include "pdft/app/view/page_processor.h"

#include <asio/strand.hpp>
#include <asio/thread_pool.hpp>

namespace pdft::app {

class AppPageProcessor : public PageProcessor, public Module {
 public:
    struct Config {
        uint32_t render_pool_size = 4;
    };

    AppPageProcessor(Config config, Arc<const ImageRenderer> renderer);

    ~AppPageProcessor() noexcept;

    void process(
        Arc<media::Page> page,
        const DisplayProperties& display,
        Promise<ProcessedImage> promise) override;

 private:
    Arc<const ImageRenderer> renderer_;
    asio::thread_pool pool_;
    asio::strand<asio::thread_pool::executor_type> media_strand_;
};

}  // namespace pdft::app
