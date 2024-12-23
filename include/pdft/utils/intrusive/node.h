#pragma once

namespace pdft::utils::intrusive {

template <typename Tag = void>
struct ForwardListItem {
    ForwardListItem<Tag>* next_ = nullptr;
};

template <typename Tag = void>
struct ListNode {
    ListNode<Tag>* next_ = nullptr;
    ListNode<Tag>* prev_ = nullptr;

    void unlink() {
        prev_->next_ = next_;
        next_->prev_ = prev_;
    }
};

}  // namespace pdft::utils::intrusive
