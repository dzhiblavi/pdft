#include "pdft/app/state/document_storage.h"
#include "pdft/core/module/register.h"

namespace pdft::app {

DocumentId DocumentStorage::addDocument(Arc<media::Document> document) {
    static DocumentId next_id = 0;

    auto id = next_id++;
    ids_by_path_.emplace(document->getPath(), id);
    documents_.emplace(id, std::move(document));
    return id;
}

bool DocumentStorage::removeDocument(DocumentId id) {
    auto it = documents_.find(id);
    if (it == documents_.end()) {
        return false;
    }

    ids_by_path_.erase(it->second->getPath());
    documents_.erase(it);
    return true;
}

Maybe<Arc<const media::Document>> DocumentStorage::getDocument(DocumentId id) const {
    auto it = documents_.find(id);
    if (it == documents_.end()) {
        return std::nullopt;
    }

    return it->second;
}

Maybe<DocumentId> DocumentStorage::getIdByPath(const std::filesystem::path& path) const {
    auto it = ids_by_path_.find(path);
    if (it == ids_by_path_.end()) {
        return std::nullopt;
    }

    return it->second;
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::DocumentStorage);
