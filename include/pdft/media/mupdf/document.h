#pragma once

#include "pdft/media/document.h"
#include "pdft/media/mupdf/context.h"

#include "pdft/core/result.h"

struct fz_document;

namespace pdft::media::mupdf {

class Document : public media::Document {
 public:
    Document(Document&& rhs);
    Document& operator=(Document&&) = delete;

    ~Document() noexcept override;

    size_t getPageCount() const override;
    Arc<media::Page> getPage(int index) const override;
    const std::filesystem::path& getPath() const override;

    static Result<Document> open(Context& context, const std::filesystem::path& path);

 private:
    Document(Context& context, std::filesystem::path path, fz_document* document)
        : context_(context)
        , path_(std::move(path))
        , document_(document) {}

    Context& context_;
    std::filesystem::path path_;
    fz_document* document_ = nullptr;
};

}  // namespace pdft::media::mupdf
