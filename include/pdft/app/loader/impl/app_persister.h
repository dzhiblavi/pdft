#pragma once

#include "pdft/app/loader/persister.h"
#include "pdft/app/state/ui.h"

#include "pdft/media/loader.h"

#include "platform/module/module.h"

namespace pdft::app {

class AppPersister : public Persister, public platform::rt::Module {
 public:
    AppPersister(platform::Arc<UI> ui, platform::Arc<DocumentStorage> documents, platform::Arc<const media::Loader> doc_loader);

    platform::Status save(std::ostream& os) const override;
    platform::Status restore(std::istream& is) const override;

 private:
    platform::Arc<UI> ui_;
    platform::Arc<DocumentStorage> documents_;
    platform::Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
