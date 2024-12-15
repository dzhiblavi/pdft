#pragma once

#include "pdft/tui/display.h"
#include "pdft/tui/kitty/apc/apc_executor.h"

#include "pdft/utils/lru_cache.h"

namespace pdft::tui {

class KittyDisplayBase : public Display {
 public:
    static constexpr size_t DefaultMaxLoadedImages = 128;

    struct Config {
        size_t max_loaded_images = DefaultMaxLoadedImages;
    };

    KittyDisplayBase(Config config, Arc<APCExecutor> executor);

    void loadCompressedImage(
        Context& ctx,
        ImageId id,
        image::ImageSize size,
        std::string_view compressed_data,
        io::OutBuffer& buffer) override;

    void clear(Context& ctx, io::OutBuffer& buffer) override;
    void unload(Context& ctx, ImageId id, io::OutBuffer& buffer) override;
    void unloadAll(Context& ctx, io::OutBuffer& buffer) override;
    bool isLoaded(ImageId id) const override;

 protected:
    Arc<APCExecutor> executor_;
    utils::LRUCache<ImageId, image::ImageSize> loaded_;
};

}  // namespace pdft::tui
