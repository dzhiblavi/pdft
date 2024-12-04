#pragma once

#include "pdft/media/document.h"
#include "pdft/media/mupdf/context.h"

#include "pdft/core/result.h"

struct fz_document;

namespace pdft::media {

class MupdfDocument : public Document {
 public:
    MupdfDocument(MupdfDocument&& rhs);
    MupdfDocument& operator=(MupdfDocument&&) = delete;

    ~MupdfDocument() noexcept override;

    size_t getPageCount() const override;
    Arc<media::Page> getPage(int index) const override;
    const std::filesystem::path& getPath() const override;

    static Result<MupdfDocument> open(MupdfContext& context, const std::filesystem::path& path);

 private:
    MupdfDocument(MupdfContext& context, std::filesystem::path path, fz_document* document)
        : context_(context)
        , path_(std::move(path))
        , document_(document) {}

    MupdfContext& context_;
    std::filesystem::path path_;
    fz_document* document_ = nullptr;
};

}  // namespace pdft::media
