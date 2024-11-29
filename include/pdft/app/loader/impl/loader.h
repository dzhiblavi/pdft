#pragma once

#include "pdft/app/loader/loader.h"
#include "pdft/app/persist/persister.h"

#include "pdft/media/loader.h"

namespace pdft::app::loader::impl {

class Loader : public loader::Loader {
 public:
    Loader(
        std::shared_ptr<const persist::Persister> persister,
        std::shared_ptr<const media::Loader> doc_loader)
        : persister_(std::move(persister))
        , doc_loader_(std::move(doc_loader)) {}

    void save(const state::UIState& ui, const state::DocumentStorage& documents, std::ostream& os)
        override;

    Result<void> load(
        const std::vector<std::string>& paths,
        state::UIState& ui,
        state::DocumentStorage& documents,
        StateMode mode,
        std::istream& is) override;

 private:
    std::shared_ptr<const persist::Persister> persister_;
    std::shared_ptr<const media::Loader> doc_loader_;
};

}  // namespace pdft::app::loader::impl
