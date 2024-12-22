#pragma once

namespace pdft::utils::intrusive {

template <typename Tag = void>
struct ForwardListItem {
    ForwardListItem<Tag>* next_ = nullptr;
};

}  // namespace pdft::utils::intrusive
