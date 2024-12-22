#pragma once

#include <atomic>
#include <functional>

#include "pdft/utils/intrusive/node.h"

namespace pdft::utils::intrusive {

// Subject to ABA problem
template <typename T, typename Tag = void>
class MPMCStack {
    using Node = ForwardListItem<Tag>;

 public:
    void push(T& item) {
        Node& node = item;
        node.next_ = head_.load(std::memory_order_relaxed);

        while (!head_.compare_exchange_weak(
            node.next_, &node, std::memory_order_release, std::memory_order_relaxed)) {
            // CAS loop
        }
    }

    T* tryPop() {
        for (;;) {
            Node* curr_head = head_.load(std::memory_order_relaxed);

            if (curr_head == nullptr) {
                return nullptr;
            }

            if (head_.compare_exchange_weak(
                    curr_head,
                    curr_head->next_,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                curr_head->next_ = nullptr;
                return upCast(curr_head);
            }
        }
    }

    void clear() {
        head_.store(nullptr, std::memory_order_relaxed);
    }

    template <typename F>
    void forEach(this auto&& self, F func) {
        auto* curr = self.head_.load(std::memory_order_relaxed);

        while (curr != nullptr) {
            auto next = curr->next_;
            std::invoke(func, *upCast(curr));
            curr = next;
        }
    }

 private:
    static T* upCast(Node* node) {
        static_assert(std::derived_from<T, ForwardListItem<Tag>>);
        return static_cast<T*>(node);
    }

    std::atomic<Node*> head_ = nullptr;
};

}  // namespace pdft::utils::intrusive
