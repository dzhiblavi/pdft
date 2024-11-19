#include "pdft/controller/app/controller.h"

#include "pdft/core/log/log.h"

namespace pdft::controller::app {

Controller::Controller(
    std::shared_ptr<state::State> state, std::shared_ptr<state::DocumentStorage> docs)
    : state_(std::move(state))
    , docs_(std::move(docs)) {
    key_tree_.bind("q", [this] { quit_ = true; });

    key_tree_.bind("j", [this] { state_->scrollNext(1); });
    key_tree_.bind("k", [this] { state_->scrollNext(-1); });
    key_tree_.bind(KeyTree::number, "j", [this](auto times) { state_->nextPage(times); });
    key_tree_.bind(KeyTree::number, "k", [this](auto times) { state_->nextPage(-times); });

    key_tree_.bind(KeyTree::number, "G", [this](auto index) {
        if (index > 0) {
            state_->setPageIndex(index - 1);
        }
    });
    key_tree_.bind("gg", [this] { state_->setPageIndex(0); });
    key_tree_.bind("G", [this] { state_->setPageIndex(INT_MAX); });

    key_tree_.bind("h", [this] { state_->nextDocument(-1); });
    key_tree_.bind("l", [this] { state_->nextDocument(+1); });
    key_tree_.bind(KeyTree::number, "h", [this](auto n) { state_->nextDocument(-n); });
    key_tree_.bind(KeyTree::number, "l", [this](auto n) { state_->nextDocument(+n); });

    key_tree_.bind("s", [this] { state_->nextLayout(1); });
    key_tree_.bind("S", [this] { state_->nextLayout(-1); });

    key_tree_.bind("=", [this] { state_->updateZoom(1.10f); });
    key_tree_.bind("-", [this] { state_->updateZoom(1.f / 1.10f); });
}

Action Controller::onKeyEvent(char key) {
    core::log::global()->info("controller: key={}", key);

    switch (key_tree_.feed(key)) {
        using enum KeyTree::Result;

        case Accepted:
            state_->onInput(key);
            return Action::Redraw;

        case Rejected:
            key_tree_.reset();
            state_->resetInput();
            return Action::Redraw;

        case Terminated:
            state_->resetInput();
            return quit_ ? Action::Quit : Action::Redraw;
    }
}

Action Controller::onFilesystemEvent(const notify::Event& event) {
    auto old_id = docs_->getIdByPath(event.path);

    core::log::global()->info(
        "controller: fs event on path={}, old_id={}", event.path.c_str(), old_id);

    docs_->removeDocument(old_id);
    auto new_id = docs_->addDocument(media::pdf::Document::open(docs_->getContext(), event.path));

    core::log::global()->info(
        "controller: replaced document with path={} old_id={}, new_id={}",
        event.path.c_str(),
        old_id,
        new_id);

    state_->onDocumentChanged(old_id, new_id);
    return Action::Redraw;
}

}  // namespace pdft::controller::app
