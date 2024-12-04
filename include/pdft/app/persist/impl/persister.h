#pragma once

#include "pdft/app/persist/persister.h"

#include "pdft/media/loader.h"

namespace pdft::app {

class AppPersister : public Persister {
 public:
    explicit AppPersister(Arc<const media::Loader> doc_loader)
        : doc_loader_(std::move(doc_loader)) {}

    void save(const UIState& ui, const DocumentStorage& documents, std::ostream& os) const override;

    Result<void> restore(UIState& ui, DocumentStorage& documents, std::istream& is) const override;

 private:
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
