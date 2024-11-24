#pragma once

#include "pdft/media/document.h"

#include <unordered_map>

namespace pdft::state {

using DocumentId = uint32_t;

class DocumentStorage {
 public:
    DocumentId addDocument(std::unique_ptr<media::Document> document);

    void removeDocument(DocumentId id);

    const media::Document& getDocument(DocumentId id) const;

    DocumentId getIdByPath(const std::filesystem::path& path) const;

 private:
    std::unordered_map<DocumentId, std::unique_ptr<media::Document>> documents_;
    std::unordered_map<std::string, DocumentId> ids_by_path_;
};

}  // namespace pdft::state
