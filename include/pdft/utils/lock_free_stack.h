#pragma once

#include <atomic>
#include <functional>

namespace pdft::utils {

template <typename Self>
struct IntrusiveForwardListItem {
    Self* next_ = nullptr;
};

template <typename T>
class MPMCLockFreeStack {
 public:
    void push(T& item) {
        item.next_ = head_.load(std::memory_order_relaxed);

        while (!head_.compare_exchange_weak(item.next_, &item)) {
            // CAS loop
        }
    }

    T* tryPop() {
        for (;;) {
            T* curr_head = head_.load(std::memory_order_relaxed);

            if (curr_head == nullptr) {
                return nullptr;
            }

            if (head_.compare_exchange_weak(curr_head, curr_head->next_)) {
                return curr_head;
            }
        }
    }

    void clear() {
        head_.store(nullptr, std::memory_order_relaxed);
    }

    template <typename F>
    void forEach(F func) {
        T* curr = head_;

        while (curr != nullptr) {
            auto next = curr->next_;
            std::invoke(func, *curr);
            curr = next;
        }
    }

    template <typename F>
    void forEach(F func) const {
        const T* curr = head_;

        while (curr != nullptr) {
            auto next = curr->next_;
            std::invoke(func, *curr);
            curr = next;
        }
    }

 private:
    std::atomic<T*> head_ = nullptr;
};

}  // namespace pdft::utils
