#pragma once

#include "pdft/app/persist/persister.h"

#include "pdft/media/loader.h"

namespace pdft::app::persist::impl {

class Persister : public persist::Persister {
 public:
    explicit Persister(std::shared_ptr<const media::Loader> doc_loader)
        : doc_loader_(std::move(doc_loader)) {}

    void save(const state::UIState& ui, const state::DocumentStorage& documents, std::ostream& os)
        const override;

    Result<void> restore(
        state::UIState& ui, state::DocumentStorage& documents, std::istream& is) const override;

 private:
    std::shared_ptr<const media::Loader> doc_loader_;
};

}  // namespace pdft::app::persist::impl
