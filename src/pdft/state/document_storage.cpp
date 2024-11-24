#include "pdft/state/document_storage.h"
#include "pdft/core/assert.h"

namespace pdft::state {

DocumentId DocumentStorage::addDocument(std::shared_ptr<media::Document> document) {
    static DocumentId next_id = 0;

    auto id = next_id++;
    ids_by_path_.emplace(document->getPath(), id);
    documents_.emplace(id, std::move(document));
    return id;
}

void DocumentStorage::removeDocument(DocumentId id) {
    auto it = documents_.find(id);
    PDFT_VERIFY(it != documents_.end(), "trying to delete an unknown document with id={}", id);
    ids_by_path_.erase(it->second->getPath());
    documents_.erase(it);
}

const media::Document& DocumentStorage::getDocument(DocumentId id) const {
    auto it = documents_.find(id);
    PDFT_VERIFY(it != documents_.end(), "trying to get an unknown document with id={}", id);
    return *it->second;
}

DocumentId DocumentStorage::getIdByPath(const std::filesystem::path& path) const {
    auto it = ids_by_path_.find(path);
    PDFT_VERIFY(
        it != ids_by_path_.end(), "no document with exists with this path={}", path.c_str());
    return it->second;
}

}  // namespace pdft::state
