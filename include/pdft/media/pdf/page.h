#include "pdft/media/pdf/context.h"

#include "pdft/display/image.h"

#include <memory>

struct fz_page;

namespace pdft::media::pdf {

struct ImageProperties {
    float scale_x = 1.f;
    float scale_y = 1.f;
    float rotation_degrees = 0.f;
    bool invert_colors = false;
};

struct PageSize {
    float width_px;
    float height_px;
};

class Page {
 public:
    Page(Page&& rhs);
    Page& operator=(Page&& rhs) = delete;

    ~Page() noexcept;

    std::unique_ptr<display::Image> getImage(const ImageProperties& properties);

    PageSize getDefaultSize() const;
    size_t getPageIndex() const;

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

}  // namespace pdft::media::pdf
