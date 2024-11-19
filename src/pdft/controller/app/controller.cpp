#include "pdft/controller/app/controller.h"

#include "pdft/core/log/log.h"

namespace pdft::controller::app {

Controller::Controller(
    std::shared_ptr<state::State> state, std::shared_ptr<state::DocumentStorage> docs)
    : state_(std::move(state))
    , docs_(std::move(docs)) {
    key_tree_.bind("q", [this] {
        action_ = Action::Quit;
    });

    key_tree_.bind("j", [this] {
        state_->scrollNext(1);
        action_ = Action::Redraw;
    });
    key_tree_.bind("k", [this] {
        state_->scrollNext(-1);
        action_ = Action::Redraw;
    });
    key_tree_.bind(KeyTree::number, "j", [this](auto times) {
        state_->nextPage(times);
        action_ = Action::Redraw;
    });
    key_tree_.bind(KeyTree::number, "k", [this](auto times) {
        state_->nextPage(-times);
        action_ = Action::Redraw;
    });

    key_tree_.bind(KeyTree::number, "G", [this](auto index) {
        state_->setPageIndex(index);
        action_ = Action::Redraw;
    });
    key_tree_.bind("gg", [this] {
        state_->setPageIndex(0);
        action_ = Action::Redraw;
    });
    key_tree_.bind("G", [this] {
        state_->setPageIndex(INT_MAX);
        action_ = Action::Redraw;
    });

    key_tree_.bind("h", [this] {
        state_->nextDocument(-1);
        action_ = Action::Redraw;
    });
    key_tree_.bind("l", [this] {
        state_->nextDocument(+1);
        action_ = Action::Redraw;
    });
    key_tree_.bind(KeyTree::number, "h", [this](auto n) {
        state_->nextDocument(-n);
        action_ = Action::Redraw;
    });
    key_tree_.bind(KeyTree::number, "l", [this](auto n) {
        state_->nextDocument(+n);
        action_ = Action::Redraw;
    });

    key_tree_.bind("s", [this] {
        state_->nextLayout(1);
        action_ = Action::Redraw;
    });
    key_tree_.bind("S", [this] {
        state_->nextLayout(-1);
        action_ = Action::Redraw;
    });

    key_tree_.bind("=", [this] {
        state_->updateZoom(1.10f);
        action_ = Action::Redraw;
    });
    key_tree_.bind("-", [this] {
        state_->updateZoom(1.f / 1.10f);
        action_ = Action::Redraw;
    });
}

Action Controller::onKeyEvent(char key) {
    core::log::global()->info("controller: key={}", key);
    action_ = Action::None;
    key_tree_.feed(key);
    return action_;
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
