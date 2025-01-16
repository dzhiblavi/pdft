#pragma once

#include "pdft/media/document.h"

#include "platform/core/result.h"

struct fz_document;  // NOLINT

namespace pdft::media {

class MupdfDocument : public Document {
 public:
    MupdfDocument(MupdfDocument&& rhs) noexcept;
    MupdfDocument& operator=(MupdfDocument&&) = delete;

    ~MupdfDocument() noexcept override;

    size_t getPageCount() const override;
    platform::Arc<media::Page> getPage(int index) const override;
    const std::filesystem::path& getPath() const override;

    static platform::Result<MupdfDocument> open(const std::filesystem::path& path);

 private:
    MupdfDocument(std::filesystem::path path, fz_document* document)
        : path_(std::move(path))
        , document_(document) {}

    std::filesystem::path path_;
    fz_document* document_ = nullptr;
};

}  // namespace pdft::media
