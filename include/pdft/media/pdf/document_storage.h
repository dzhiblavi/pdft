#pragma once

#include "pdft/media/pdf/document.h"

#include <cstdint>
#include <unordered_map>

namespace pdft::media::pdf {

using DocumentId = uint32_t;

class DocumentStorage {
 public:
    explicit DocumentStorage(std::shared_ptr<Context> context) : context_(std::move(context)) {}

    DocumentId addDocument(Document document);

    void removeDocument(DocumentId id);

    const Document& getDocument(DocumentId id) const;

    DocumentId getIdByPath(const std::filesystem::path& path) const;

    Context& getContext() const;

 private:
    std::shared_ptr<Context> context_;
    std::unordered_map<DocumentId, Document> documents_;
    std::unordered_map<std::string, DocumentId> ids_by_path_;
};

}  // namespace pdft::media::pdf
