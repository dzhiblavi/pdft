#pragma once

#include "pdft/app/view/display_properties.h"
#include "pdft/media/page.h"

#include "platform/core/future.h"
#include "platform/core/rc.h"

namespace pdft::app {

struct ProcessedImage {
    image::ImageSize size;
    std::string compressed_samples;
};

class PageProcessor {
 public:
    virtual ~PageProcessor() = default;

    virtual void process(
        platform::Arc<media::Page> page,
        const DisplayProperties& display,
        platform::Promise<ProcessedImage> promsise) = 0;
};

}  // namespace pdft::app
