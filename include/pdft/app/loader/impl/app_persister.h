#pragma once

#include "pdft/app/loader/persister.h"
#include "pdft/app/state/ui.h"

#include "pdft/media/loader.h"

#include "pdft/core/module/module.h"

namespace pdft::app {

class AppPersister : public Persister, public Module {
 public:
    AppPersister(Arc<UI> ui, Arc<DocumentStorage> documents, Arc<const media::Loader> doc_loader);

    Status save(std::ostream& os) const override;
    Status restore(std::istream& is) const override;

 private:
    Arc<UI> ui_;
    Arc<DocumentStorage> documents_;
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
