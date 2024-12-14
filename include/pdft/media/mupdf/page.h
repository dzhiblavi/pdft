#pragma once

#include "pdft/image/image.h"
#include "pdft/media/page.h"

struct fz_page;  // NOLINT

namespace pdft::media {

class MupdfPage : public Page {
 public:
    MupdfPage(MupdfPage&& rhs) noexcept;
    MupdfPage& operator=(MupdfPage&& rhs) = delete;

    ~MupdfPage() noexcept override;

    Arc<image::Image> getImage(float scaling_factor) const override;

    PageSize getDefaultSize() const override;
    size_t getPageIndex() const override;

 private:
    MupdfPage(size_t page_index, fz_page* page) : page_index_(page_index), page_(page) {}

    size_t page_index_;
    fz_page* page_ = nullptr;

    friend class MupdfDocument;
};

}  // namespace pdft::media
