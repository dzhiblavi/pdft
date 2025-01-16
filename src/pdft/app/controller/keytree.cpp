#include "pdft/app/controller/impl/keytree.h"

#include "platform/core/assert.h"

#include <stack>

namespace pdft::app {

KeyTree::~KeyTree() noexcept {
    std::stack<Node*> nodes;

    if (root_ != nullptr) {
        nodes.push(root_);
    }

    if (num_root_ != nullptr) {
        nodes.push(num_root_);
    }

    while (!nodes.empty()) {
        auto* top = nodes.top();
        nodes.pop();

        for (auto [_, node] : top->children) {
            nodes.push(node);
        }

        delete top;
    }
}

void KeyTree::bind(std::string_view keys, CallbackType callback) {
    if (root_ == nullptr) {
        root_ = new Node;
    }

    bindFrom(root_, keys, [cb = std::move(callback)](auto /*ignored*/) { std::move(cb)(); });
}

void KeyTree::bind(NumberType, std::string_view keys, NumberCallbackType callback) {
    if (num_root_ == nullptr) {
        num_root_ = new Node;
    }

    bindFrom(num_root_, keys, std::move(callback));
}

void KeyTree::bindFrom(Node* root, std::string_view keys, NumberCallbackType callback) {
    platform::verify(!keys.empty(), "cannot bind empty key sequence");

    Node* curr = root;

    for (char c : keys) {
        auto it = curr->children.find(c);

        if (it != curr->children.end()) {
            curr = it->second;
        } else {
            Node* next = new Node;
            curr->children[c] = next;
            curr = next;
        }

        platform::verify(!curr->callback, "old keybinding is a prefix of new one: {}", keys);
    }

    curr->callback = std::move(callback);
}

KeyTree::Result KeyTree::feed(char key) {
    if (std::isdigit(key)) {
        return feedDigit(key);
    } else {
        return feedChar(key);
    }
}

KeyTree::Result KeyTree::feedChar(char key) {
    switch (state_) {
        case None:
            if (root_ == nullptr) {
                return Result::Rejected;
            }
            break;

        case Simple:
            break;

        case Counting:
            state_ = Simple;
            break;
    }

    auto curr = curr_ != nullptr ? curr_ : root_;
    auto it = curr->children.find(key);

    if (it == curr->children.end()) {
        return Result::Rejected;
    }

    curr_ = it->second;
    state_ = Simple;

    if (!curr_->callback) {
        return Result::Accepted;
    }

    curr_->callback(number_);
    reset();
    return Result::Terminated;
}

KeyTree::Result KeyTree::feedDigit(char key) {
    int digit = key - '0';

    switch (state_) {
        case None:
            platform::verify(curr_ == nullptr, "invalid state");
            if (num_root_ != nullptr) {
                curr_ = num_root_;
                state_ = Counting;
                number_ = digit;
                return Result::Accepted;
            } else {
                return feedChar(key);
            }

        case Simple:
            platform::verify(curr_ != nullptr, "invalid state");
            return feedChar(key);

        case Counting:
            platform::verify(curr_ == num_root_, "invalid state");
            number_ = number_ * 10 + digit;  // NOLINT
            return Result::Accepted;
    }
}

void KeyTree::reset() {
    curr_ = nullptr;
    state_ = State::None;
}

}  // namespace pdft::app
