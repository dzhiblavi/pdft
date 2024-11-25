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
    std::string_view samples() const override;

 private:
    PixmapImage(Context& context, fz_pixmap* pixmap) : context_(context), pixmap_(pixmap) {}

    Context& context_;
    fz_pixmap* pixmap_;

    friend class Page;
};

}  // namespace pdft::media::pdf
