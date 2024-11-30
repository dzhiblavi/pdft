#pragma once

#include "pdft/media/document.h"

#include <unordered_map>

namespace pdft::app::state {

using DocumentId = uint32_t;

class DocumentStorage {
 public:
    DocumentId addDocument(std::shared_ptr<media::Document> document);

    [[nodiscard]] bool removeDocument(DocumentId id);

    std::optional<std::shared_ptr<const media::Document>> getDocument(DocumentId id) const;

    std::optional<DocumentId> getIdByPath(const std::filesystem::path& path) const;

 private:
    std::unordered_map<DocumentId, std::shared_ptr<media::Document>> documents_;
    std::unordered_map<std::string, DocumentId> ids_by_path_;
};

}  // namespace pdft::app::state
