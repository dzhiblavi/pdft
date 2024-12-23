#pragma once

#include "pdft/utils/intrusive/node.h"

#include <cstddef>
#include <iterator>

namespace pdft::utils::intrusive {

template <typename T, typename Tag = void>
class List;

template <typename T, typename Tag>
struct ListIterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;

    ListIterator() = default;
    ListIterator(ListNode<Tag>* ptr) : node_(ptr) {}

    reference operator*() const {
        return *static_cast<T*>(node_);
    }

    pointer operator->() {
        return static_cast<T*>(node_);
    }

    ListIterator& operator++() {
        node_ = node_->next_;
        return *this;
    }

    ListIterator& operator--() {
        node_ = node_->prev_;
        return *this;
    }

    ListIterator operator++(int) {
        auto tmp = *this;
        return ++tmp;
    }

    ListIterator operator--(int) {
        auto tmp = *this;
        return --tmp;
    }

    bool operator==(const ListIterator& a) const = default;

 private:
    ListNode<Tag>* node_ = nullptr;

    friend class List<T, Tag>;
};

static_assert(std::bidirectional_iterator<ListIterator<int, int>>);

template <typename T, typename Tag>
class List {
    using Node = ListNode<Tag>;

 public:
    using iterator_type = ListIterator<T, Tag>;

    List() = default;

    void push_back(T& item) {  // NOLINT
        insert(end(), item);
    }

    void push_front(T& item) {  // NOLINT
        insert(begin(), item);
    }

    void insert(iterator_type before, T& item) {
        Node* node = before.node_;
        Node* prev = node->prev_;

        link(prev, &item);
        link(&item, node);
    }

    void pop_back() {  // NOLINT
        erase(std::prev(end()));
    }

    void pop_front() {  // NOLINT
        erase(begin());
    }

    void erase(iterator_type iter) {
        Node* node = iter.node_;
        Node* prev = node->prev_;
        Node* next = node->next_;

        link(prev, next);
        node->next_ = node->prev_ = nullptr;
    }

    void clear() {
        sentinel_ = {&sentinel_, &sentinel_};
    }

    bool empty() const {
        return sentinel_.next_ == &sentinel_;
    }

    iterator_type begin() {
        return {sentinel_.next_};
    }

    iterator_type end() {
        return {&sentinel_};
    }

 private:
    static void link(Node* a, Node* b) {
        a->next_ = b;
        b->prev_ = a;
    }

    Node sentinel_{&sentinel_, &sentinel_};
};

}  // namespace pdft::utils::intrusive
