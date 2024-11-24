#pragma once

#include "pdft/media/pdf/document.h"

#include <unordered_map>

namespace pdft::state {

using DocumentId = uint32_t;

class DocumentStorage {
 public:
    explicit DocumentStorage(std::shared_ptr<media::pdf::Context> context)
        : context_(std::move(context)) {}

    DocumentId addDocument(media::pdf::Document document);

    void removeDocument(DocumentId id);

    const media::pdf::Document& getDocument(DocumentId id) const;

    DocumentId getIdByPath(const std::filesystem::path& path) const;

    media::pdf::Context& getContext() const;

 private:
    std::shared_ptr<media::pdf::Context> context_;
    std::unordered_map<DocumentId, media::pdf::Document> documents_;
    std::unordered_map<std::string, DocumentId> ids_by_path_;
};

}  // namespace pdft::state
