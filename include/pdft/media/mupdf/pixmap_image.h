#pragma once

#include "pdft/media/mupdf/context.h"

#include "pdft/image/image.h"

struct fz_pixmap;

namespace pdft::media::mupdf {

class PixmapImage : public image::Image {
 public:
    PixmapImage(PixmapImage&&);
    PixmapImage& operator=(PixmapImage&&) = delete;

    ~PixmapImage() noexcept override;

    image::ImageSize size() const override;
    std::span<const uint8_t> samples() const override;
    std::span<uint8_t> mutableSamples() override;
    const image::ImageProperties& getProperties() const override;

 private:
    PixmapImage(Context& context, fz_pixmap* pixmap, image::ImageProperties properties)
        : context_(context)
        , pixmap_(pixmap)
        , properties_(properties) {}

    Context& context_;
    fz_pixmap* pixmap_;
    image::ImageProperties properties_;

    friend class Page;
};

}  // namespace pdft::media::mupdf
