#pragma once

#include "pdft/app/loader/persister.h"
#include "pdft/media/loader.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

namespace pdft::app {

class AppPersister : public Persister, public Module {
 public:
    explicit AppPersister(inject<const media::Loader, "media_loader"> doc_loader)
        : doc_loader_(std::move(doc_loader)) {}

    void save(const UIState& ui, const DocumentStorage& documents, std::ostream& os) const override;

    Status restore(UIState& ui, DocumentStorage& documents, std::istream& is) const override;

 private:
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
