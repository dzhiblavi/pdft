#pragma once

#include <atomic>
#include <functional>

namespace pdft::utils::intrusive {

// Subject to ABA problem
template <typename T>
class MPMCStack {
 public:
    void push(T& item) {
        item.next_ = head_.load(std::memory_order_relaxed);

        while (!head_.compare_exchange_weak(
            item.next_, &item, std::memory_order_release, std::memory_order_relaxed)) {
            // CAS loop
        }
    }

    T* tryPop() {
        for (;;) {
            T* curr_head = head_.load(std::memory_order_relaxed);

            if (curr_head == nullptr) {
                return nullptr;
            }

            if (head_.compare_exchange_weak(
                    curr_head,
                    curr_head->next_,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                curr_head->next_ = nullptr;
                return curr_head;
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
            std::invoke(func, *curr);
            curr = next;
        }
    }

 private:
    std::atomic<T*> head_ = nullptr;
};

}  // namespace pdft::utils::intrusive
