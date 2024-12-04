#pragma once

#include "pdft/media/mupdf/context.h"

#include "pdft/image/image.h"
#include "pdft/media/page.h"

struct fz_page;

namespace pdft::media {

class MupdfPage : public Page {
 public:
    MupdfPage(MupdfPage&& rhs);
    MupdfPage& operator=(MupdfPage&& rhs) = delete;

    ~MupdfPage() noexcept override;

    Arc<image::Image> getImage(float scaling_factor) const override;

    PageSize getDefaultSize() const override;
    size_t getPageIndex() const override;

 private:
    MupdfPage(size_t page_index, MupdfContext& context, fz_page* page)
        : page_index_(page_index)
        , context_(context)
        , page_(page) {}

    size_t page_index_;
    MupdfContext& context_;
    fz_page* page_ = nullptr;

    friend class MupdfDocument;
};

}  // namespace pdft::media
