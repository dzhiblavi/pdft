#pragma once

#include "pdft/media/document.h"

#include "pdft/core/result/type/result.h"

#include <unordered_map>

namespace pdft::app::state {

using DocumentId = uint32_t;

class DocumentStorage {
 public:
    DocumentId addDocument(Arc<media::Document> document);

    [[nodiscard]] bool removeDocument(DocumentId id);

    Maybe<Arc<const media::Document>> getDocument(DocumentId id) const;

    Maybe<DocumentId> getIdByPath(const std::filesystem::path& path) const;

 private:
    std::unordered_map<DocumentId, Arc<media::Document>> documents_;
    std::unordered_map<std::string, DocumentId> ids_by_path_;
};

}  // namespace pdft::app::state
