#include "pdft/controller/keytree.h"
#include "pdft/core/assert.h"

#include <stack>

namespace pdft::controller {

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

    bindFrom(root_, keys, [cb = std::move(callback)](auto /*ignored*/) {
        std::move(cb)();
    });
}

void KeyTree::bind(NumberType, std::string_view keys, NumberCallbackType callback) {
    if (num_root_ == nullptr) {
        num_root_ = new Node;
    }

    bindFrom(num_root_, keys, std::move(callback));
}

void KeyTree::bindFrom(Node* root, std::string_view keys, NumberCallbackType callback) {
    PDFT_VERIFY(!keys.empty());

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

        PDFT_VERIFY(!curr->callback);
    }

    curr->callback = std::move(callback);
}

void KeyTree::feed(char key) {
    if (std::isdigit(key)) {
        feedDigit(key);
    } else {
        feedChar(key);
    }
}

void KeyTree::feedChar(char key) {
    switch (state_) {
        case None:
            if (root_ != nullptr) {
                state_ = Simple;
                curr_ = root_;
            } else {
                return;
            }
            break;

        case Simple:
            break;

        case Counting:
            state_ = Simple;
            break;
    }

    auto it = curr_->children.find(key);
    if (it == curr_->children.end()) {
        auto* curr = curr_;
        reset();

        bool is_digit = std::isdigit(key);
        if ((is_digit && num_root_ != curr) || (!is_digit && root_ != curr)) {
            feed(key);
        }

        return;
    }

    auto* next = it->second;
    if (next->callback) {
        next->callback(number_);
        reset();
    } else {
        curr_ = next;
    }
}

void KeyTree::feedDigit(char key) {
    int digit = key - '0';

    switch (state_) {
        case None:
            PDFT_VERIFY(curr_ == nullptr);
            if (num_root_ != nullptr) {
                curr_ = num_root_;
                state_ = Counting;
                number_ = digit;
            } else {
                feedChar(key);
            }
            break;

        case Simple:
            PDFT_VERIFY(curr_ != nullptr);
            feedChar(key);
            break;

        case Counting:
            PDFT_VERIFY(curr_ == num_root_);
            number_ = number_ * 10 + digit;
            break;
    }
}

void KeyTree::reset() {
    curr_ = nullptr;
    state_ = State::None;
}

}  // namespace pdft::controller
