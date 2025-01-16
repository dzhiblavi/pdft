#pragma once

#include "pdft/media/document.h"

#include "platform/core/result.h"
#include "platform/module/module.h"

#include <unordered_map>

namespace pdft::app {

using DocumentId = uint32_t;

class DocumentStorage : public platform::rt::Module {
 public:
    DocumentId addDocument(platform::Arc<media::Document> document);

    [[nodiscard]] bool removeDocument(DocumentId id);

    platform::Maybe<platform::Arc<const media::Document>> getDocument(DocumentId id) const;

    platform::Maybe<DocumentId> getIdByPath(const std::filesystem::path& path) const;

 private:
    std::unordered_map<DocumentId, platform::Arc<media::Document>> documents_;
    std::unordered_map<std::string, DocumentId> ids_by_path_;
};

}  // namespace pdft::app
