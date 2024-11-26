#pragma once

#include "pdft/core/assert.h"

#include <functional>

namespace pdft::utils {

template <typename Self>
struct IntrusiveForwardListItem {
    Self* next_ = nullptr;
};

template <typename T>
class IntrusiveForwardList {
 public:
    void pushFront(T& item) {
        if (isEmpty()) {
            size_ = 1;
            head_ = &item;
            head_->next_ = nullptr;
            return;
        }

        ++size_;
        item.next_ = head_;
        head_ = &item;
    }

    T& front() {
        PDFT_VERIFY(!isEmpty());
        return *head_;
    }

    const T& front() const {
        PDFT_VERIFY(!isEmpty());
        return *head_;
    }

    T& popFront() {
        PDFT_VERIFY(!isEmpty());

        auto item = head_;
        --size_;
        head_ = head_->next_;

        return *item;
    }

    void clear() {
        head_ = nullptr;
        size_ = 0;
    }

    bool isEmpty() const {
        return size() == 0;
    }

    size_t size() const {
        return size_;
    }

    template <typename F>
    void forEachReversed(F func) {
        T* curr = head_;

        while (curr != nullptr) {
            auto next = curr->next_;
            std::invoke(func, *curr);
            curr = next;
        }
    }

    template <typename F>
    void forEach(F func) {
        if (size() == 0) {
            return;
        }

        std::vector<T*> values;
        values.resize(size());
        size_t index = size() - 1;
        forEachReversed([&](T& item) { values[index--] = &item; });

        for (auto* item : values) {
            std::invoke(func, *item);
        }
    }

    template <typename F>
    void forEachReversed(F func) const {
        const T* curr = head_;

        while (curr != nullptr) {
            auto next = curr->next_;
            std::invoke(func, *curr);
            curr = next;
        }
    }

    template <typename F>
    void forEach(F func) const {
        if (size() == 0) {
            return;
        }

        std::vector<const T*> values;
        values.resize(size());
        size_t index = size() - 1;
        forEachReversed([&](const T& item) { values[index--] = &item; });

        for (auto* item : values) {
            std::invoke(func, *item);
        }
    }

 private:
    T* head_ = nullptr;
    size_t size_ = 0;
};

}  // namespace pdft::utils
