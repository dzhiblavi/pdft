#pragma once

#include "pdft/app/loader/loader.h"
#include "pdft/app/persist/persister.h"

#include "pdft/media/loader.h"

namespace pdft::app {

class AppLoader : public Loader {
 public:
    AppLoader(Arc<const Persister> persister, Arc<const media::Loader> doc_loader)
        : persister_(std::move(persister))
        , doc_loader_(std::move(doc_loader)) {}

    void save(const UIState& ui, const DocumentStorage& documents, std::ostream& os) override;

    Status load(
        const std::vector<std::string>& paths,
        UIState& ui,
        DocumentStorage& documents,
        StateMode mode,
        std::istream& is) override;

 private:
    Arc<const Persister> persister_;
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
