#pragma once

#include "pdft/media/pdf/context.h"
#include "pdft/media/pdf/page.h"

#include <filesystem>

struct fz_document;

namespace pdft::media::pdf {

class Document {
 public:
    ~Document() noexcept;

    Document(Document&& rhs);
    Document& operator=(Document&&) = delete;

    int getPageCount() const;
    Page getPage(int index) const;
    const std::filesystem::path& getPath() const;

    static Document open(Context& context, const std::filesystem::path& path);

 private:
    Document(Context& context, std::filesystem::path path, fz_document* document)
        : context_(context)
        , path_(std::move(path))
        , document_(document) {}

    Context& context_;
    std::filesystem::path path_;
    fz_document* document_ = nullptr;
};

}  // namespace pdft::media::pdf
