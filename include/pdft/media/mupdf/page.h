#pragma once

#include "pdft/media/mupdf/context.h"

#include "pdft/image/image.h"
#include "pdft/media/page.h"

struct fz_page;

namespace pdft::media::mupdf {

class Page : public media::Page {
 public:
    Page(Page&& rhs);
    Page& operator=(Page&& rhs) = delete;

    ~Page() noexcept override;

    Arc<image::Image> getImage(float scaling_factor) const override;

    PageSize getDefaultSize() const override;
    size_t getPageIndex() const override;

 private:
    Page(size_t page_index, Context& context, fz_page* page)
        : page_index_(page_index)
        , context_(context)
        , page_(page) {}

    size_t page_index_;
    Context& context_;
    fz_page* page_ = nullptr;

    friend class Document;
};

}  // namespace pdft::media::mupdf
