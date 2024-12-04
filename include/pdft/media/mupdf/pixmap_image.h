#pragma once

#include "pdft/media/mupdf/context.h"

#include "pdft/image/image.h"

struct fz_pixmap;

namespace pdft::media {

class PixmapMupdfImage : public image::Image {
 public:
    PixmapMupdfImage(PixmapMupdfImage&&);
    PixmapMupdfImage& operator=(PixmapMupdfImage&&) = delete;

    ~PixmapMupdfImage() noexcept override;

    image::ImageSize size() const override;
    std::span<const image::RGBColor> samples() const override;
    std::span<image::RGBColor> mutableSamples() override;

 private:
    PixmapMupdfImage(MupdfContext& context, fz_pixmap* pixmap)
        : context_(context)
        , pixmap_(pixmap) {}

    MupdfContext& context_;
    fz_pixmap* pixmap_;

    friend class MupdfPage;
};

}  // namespace pdft::media
