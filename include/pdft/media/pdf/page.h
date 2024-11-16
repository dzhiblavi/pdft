#include "context.h"
#include "pdft/media/image.h"

#include <memory>

struct fz_page;

namespace pdft::media::pdf {

using PageId = uint32_t;

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

    std::unique_ptr<Image> getImage(const ImageProperties& properties);

    PageSize getDefaultSize() const;

 private:
    Page(Context& context, fz_page* page) : context_(context), page_(page) {}

    Context& context_;
    fz_page* page_ = nullptr;

    friend class Document;
};

}  // namespace pdft::media::pdf
