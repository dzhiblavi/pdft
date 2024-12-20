#pragma once

namespace pdft::utils::intrusive {

template <typename Self>
struct ForwardListItem {
    Self* next_ = nullptr;
};

}  // namespace pdft::utils::intrusive
