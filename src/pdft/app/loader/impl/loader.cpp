#include "pdft/app/loader/impl/loader.h"
#include "pdft/core/assert.h"

#include "pdft/media/mupdf/context.h"
#include "pdft/media/mupdf/document.h"

#include <rfl/json.hpp>

#include <vector>

namespace pdft::app::loader::impl {

namespace {

struct State {
    state::UIState ui;
    std::vector<std::string> paths;
};

}  // namespace

void Loader::save(
    const state::UIState& ui, const state::DocumentStorage& documents, std::ostream& os) {
    State state;
    state.ui = ui;

    for (const auto& doc : ui.states) {
        auto id = doc.document_id;
        state.paths.push_back(documents.getDocument(id).getPath());
    }

    rfl::json::write(state, os);
}

Result<void> Loader::restore(
    state::UIState& ui, state::DocumentStorage& documents, std::istream& is) {
    auto maybe_state = rfl::json::read<State>(is);

    if (!maybe_state) {
        return std::unexpected(maybe_state.error()->what());
    }

    auto&& state = maybe_state.value();
    ui = state.ui;

    // XXX Do not abort when file does not exist
    for (size_t i = 0; i < state.paths.size(); ++i) {
        auto&& path = state.paths[i];

        if (!std::filesystem::exists(path)) {
            return std::unexpected(std::format("file does not exist: {}", path.c_str()));
        }

        auto doc = std::make_shared<media::mupdf::Document>(
            media::mupdf::Document::open(media::mupdf::thisThreadContext(), path));

        auto id = documents.addDocument(doc);
        ui.states[i].setDocument(id, doc->getPageCount());
    }

    return {};
}

}  // namespace pdft::app::loader::impl
