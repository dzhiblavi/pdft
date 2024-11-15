#include "pdft/media/pdf/document_storage.h"
#include "pdft/core/assert.h"

namespace pdft::media::pdf {

DocumentId DocumentStorage::addDocument(Document document) {
    static DocumentId next_id = 0;

    auto id = next_id++;
    ids_by_path_.emplace(document.getPath(), id);
    documents_.emplace(id, std::move(document));
    return id;
}

void DocumentStorage::removeDocument(DocumentId id) {
    auto it = documents_.find(id);
    PDFT_VERIFY(it != documents_.end());
    ids_by_path_.erase(it->second.getPath());
    documents_.erase(it);
}

const Document& DocumentStorage::getDocument(DocumentId id) const {
    auto it = documents_.find(id);
    PDFT_VERIFY(it != documents_.end());
    return it->second;
}

DocumentId DocumentStorage::getIdByPath(const std::filesystem::path& path) const {
    auto it = ids_by_path_.find(path);
    PDFT_VERIFY(it != ids_by_path_.end());
    return it->second;
}

Context& DocumentStorage::getContext() const {
    return *context_;
}

}  // namespace pdft::media::pdf
