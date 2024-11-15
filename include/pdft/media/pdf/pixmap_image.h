#pragma once

#include "context.h"
#include "pdft/media/image.h"

struct fz_pixmap;

namespace pdft::media::pdf {

class PixmapImage : public media::Image {
 public:
    PixmapImage(PixmapImage&&);
    PixmapImage& operator=(PixmapImage&&) = delete;
    ~PixmapImage() noexcept;

    ImageSize size() const override;
    std::string_view samples() const override;

 private:
    PixmapImage(Context& context, fz_pixmap* pixmap) : context_(context), pixmap_(pixmap) {}

    Context& context_;
    fz_pixmap* pixmap_;

    friend class Page;
};

}  // namespace pdft::media::pdf
